//
// Summary: server core source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-08-27 15:43:28.
// File name: event_loop.cc
//
// Description: 
// Define class EventLoop, It's a reactor.
// With Epoll, EventLoop uses to dispose a large number of channel(I/O event)
// in a thread.
//

#include "core/configure.h"

// #include <sys/eventfd.h>

#include <boost/bind.hpp>

#include "core/channel.h"
#include "core/epoller.h"
#include "core/event_loop.h"
#include "core/event_control.h"
#include "core/event_handler.h"
#include "core/event_pipe.h"
#include "core/signal_set.h"
#include "core/timing_wheel.h"
#include "core/timer_handler.h"
#include "core/base/logging.h"
#include "core/base/auto_mutex.h"
#ifdef _TIME_CONSUMING_TEST
#include "../global/time_consuming.h"
#endif


namespace core {

EventLoop::EventLoop(int poll_timeout_msec)
       : thread_id_(0), epoller_(NULL),
         wheel_(NULL), event_(NULL),
         poll_timeout_msec_(poll_timeout_msec),
         wakeup_channel_(NULL) {}

EventLoop::~EventLoop() {
  if(this->event_) {
    delete this->event_;
    this->event_ = NULL;
  }
  if(this->wheel_) {
    delete this->wheel_;
    this->wheel_ = NULL;
  }
  if(this->epoller_) {
    delete this->epoller_;
    this->epoller_ = NULL;
  }
  if(wakeup_channel_) {
    delete this->wakeup_channel_;
    this->wakeup_channel_ = NULL;
  }
  CloseEventPipe(this->wakeup_fd_);
}

bool EventLoop::Initialize(bool timer) {
  // Update time.
  this->time_.Update();

  // Resize the active channel.
  this->active_channels_.resize(128);

  // Create and allocate event pipe.
  CreateEventPipe(this->wakeup_fd_);
  this->wakeup_channel_ = new (std::nothrow) Channel(this, EventPipeReadfd(this->wakeup_fd_));
  if(this->wakeup_channel_ == NULL) {
    CoreLog(FATAL, "%s:%d (%s) Failed to create wakeup_channel_",
           __FILE__, __LINE__, __FUNCTION__);
  }

  // Allocate heap of epoll, signal and timing-wheel.
  this->epoller_ = new (std::nothrow) Epoller(this);
  if(this->epoller_ == NULL) {
    CoreLog(ERROR, "%s:%d (%s) Failed in allocate epoller_.", __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  if(timer) {
    this->wheel_ = new (std::nothrow) TimingWheel(Configure::GetTimer()->unit_interval_msec_, this);
    if(this->wheel_ == NULL || this->wheel_->Start(TimestampMillisecond(this->time_)) == false) {
      CoreLog(ERROR, "%s:%d (%s) Failed in allocate wheel_ or start error.",
             __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
  }
  this->event_ = new (std::nothrow) EventControl(this);
  if(this->event_ == NULL || this->event_->Initialize() == false || this->event_->Start() == false) {
    CoreLog(ERROR, "%s:%d (%s) Failed in allocate event_ or Initialize error.",
           __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  this->wakeup_channel_->SetReadCallback(
         boost::bind(&EventLoop::HandleRead, this, _1));
  this->wakeup_channel_->EnableRead();

  return true;
}

void EventLoop::Loop() {
  this->looping_ = true;
  this->quit_ = false;

  this->thread_id_ = current_thread::tid();

  int events_number = 0;

  while(this->quit_ == false) {
    // Get time.
    this->time_.Update();
    events_number = this->epoller_->Poll(&this->active_channels_[0],
           this->active_channels_.size(), this->poll_timeout_msec_);
    if(events_number < 0) {
      continue;
    }
    if((size_t)events_number == this->active_channels_.size()) {
      // Allocate 2 multiples of size.
      this->active_channels_.resize(this->active_channels_.size() * 2);
    }
    int pos = 0;
    for(; pos < events_number; ++pos) {
      if(this->active_channels_[pos].data.fd == 0 || this->active_channels_[pos].events == 0) continue;
      Channel *channel = static_cast<Channel *>(this->active_channels_[pos].data.ptr);
      assert(channel);
      channel->SetHappendEvent(this->active_channels_[pos].events);
      // Callback.
      channel->HandleEvent(this->time_);
    }

    this->time_.Update();

    // Time callbacks.
    this->DoTimeCallbacks(this->time_);

    // Wakeup callback.
    this->DoCallbacks();

    // For debug.
    // usleep(0);
  }

  this->quit_ = true;
  this->looping_ = false;
}

void EventLoop::AddTimer(uint32 id, uint32 msecs, int call_times,
       TimerHandler *handler, const std::string &debug) {
  this->wheel_->Schedule(id, msecs / Configure::GetTimer()->unit_interval_msec_,
         call_times, handler, debug);
}

void EventLoop::RemoveTimer(uint32 id, TimerHandler *handler) {
  this->wheel_->Unschedule(id, handler);
}

void EventLoop::SetEvent(const EventHandler *handler) {
  this->event_->SetEvent(handler);
}

void EventLoop::AddChannel(Channel* channel) {
  assert(channel);
  this->epoller_->AddChannel(channel);
}

void EventLoop::RemoveChannel(Channel* channel) {
  assert(channel);
  CoreLog(INFO, "EventLoop::RemoveChannel");
  this->epoller_->RemoveChannel(channel);
}

void EventLoop::UpdateChannel(Channel* channel) {
  assert(channel);
  this->epoller_->UpdateChannel(channel);
}

void EventLoop::WakeupCallback(const EventLoopCallback &callback, bool callback_immediate) {
  if(callback_immediate && this->CheckInLoopThread()) {
    callback();
  } else {
    {
      AutoMutex auto_mutex(&this->wakeup_mutex_);
      this->wakeup_callbacks_.push_back(callback);
    }
    this->Wakeup();
  }
}

void EventLoop::TailCallback(const EventLoopCallback &callback) {
  AutoMutex auto_mutex(&this->tail_mutex_);
  this->tail_callbacks_.push_back(callback);
}

void EventLoop::TimeCallback(const EventLoopTimeCallback &callback) {
  AutoMutex auto_mutex(&this->time_mutex_);
  this->time_callbacks_.push_back(callback);
}

void EventLoop::HandleRead(const TimeTick &time) {
  uint64 data = 1;
  ssize_t n = ::read(EventPipeReadfd(this->wakeup_fd_), &data, sizeof(data));
  if(n != sizeof(data)) {
    CoreLog(ERROR, "%s:%d (%s) wakeup and read [%u] bytes data.",
          __FILE__, __LINE__, __FUNCTION__, n);
  }
}

void EventLoop::Wakeup() {
  uint64 data = 1;
  ssize_t n = ::write(EventPipeWritefd(this->wakeup_fd_), &data, sizeof(data));
  if(n != sizeof(data)) {
    CoreLog(ERROR, "%s:%d (%s) wakeup and write [%u] bytes data.",
          __FILE__, __LINE__, __FUNCTION__, n);
  }
}

void EventLoop::DoWakeupCallbacks() {
  // Shorten the time of lock.
  this->wakeup_mutex_.Lock();
  FunctorList callbacks(this->wakeup_callbacks_);
  this->wakeup_callbacks_.clear();
  this->wakeup_mutex_.Unlock();

  FunctorList::iterator iter = callbacks.begin();
  for(; iter != callbacks.end(); ++iter) {
    (*iter)();
  }
}

void EventLoop::DoTailCallbacks() {
  // Shorten the time of lock.
  this->tail_mutex_.Lock();
  FunctorList callbacks(this->tail_callbacks_);
  this->tail_callbacks_.clear();
  this->tail_mutex_.Unlock();

  FunctorList::iterator iter = callbacks.begin();
  for(; iter != callbacks.end(); ++iter) {
    (*iter)();
  }
}

// callback with a time.
void EventLoop::DoTimeCallbacks(const TimeTick &time_tick) {
  this->time_mutex_.Lock();
  TimeFunctorList callbacks(this->time_callbacks_);
  this->time_mutex_.Unlock();

  TimeFunctorList::iterator iterator = callbacks.begin();
  for(; iterator != callbacks.end(); ++iterator) {
    (*iterator)(time_tick);
  }
}

}  // namespace core

