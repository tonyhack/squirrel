//
// Summary: server core source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-08-27 15:42:48.
// File name: event_control.cc
//
// Description: 
// Define class EventControl.
//

#include <boost/bind.hpp>

#include "core/channel.h"
#include "core/event_control.h"
#include "core/event_handler.h"
#include "core/event_loop.h"
#include "core/event_pipe.h"

namespace core {

EventControl::EventControl(EventLoop *loop)
       : channel_(NULL), loop_(loop) { assert(this->loop_); }

EventControl::~EventControl() {
  if(this->channel_) {
    this->channel_->DisableAll();
    ::close(this->channel_->GetFd());
    delete this->channel_;
  }
}

bool EventControl::Initialize() {
  CreateEventPipe(this->fd_);
  this->channel_ = new (std::nothrow) Channel(this->loop_, EventPipeReadfd(this->fd_));

  if(this->channel_ == NULL) {
    CoreLog(FATAL, "%s:%d (%s) Failed in allocate channel.",
           __FILE__, __LINE__, __FUNCTION__);
    EventPipeReadfd(this->fd_);
    return false;
  }

  return true;
}

bool EventControl::Start() {
  assert(this->channel_);
  this->channel_->SetReadCallback(boost::bind(&EventControl::OnReadCallback, this, _1));
  this->channel_->EnableRead();

  return true;
}

void EventControl::SetEvent(const EventHandler *handler) {
  assert(handler);
  ssize_t n = ::write(EventPipeWritefd(this->fd_), (uint64 *)handler, sizeof(uint64));
  if(n != sizeof(uint64)) {
    CoreLog(ERROR, "%s:%d (%s) write [%u] bytes data.",
          __FILE__, __LINE__, __FUNCTION__, n);
  }
}

void EventControl::OnReadCallback(const TimeTick &time_tick) {
  uint64 data = 0;
  ssize_t n = ::read(EventPipeReadfd(this->fd_), &data, sizeof(data));
  if(n == sizeof(data)) {
    EventHandler *handler = (EventHandler *)data;
    assert(handler);
    handler->OnEventCallback();
  } else {
    CoreLog(ERROR, "%s:%d (%s) wakeup and read [%u] bytes data.",
           __FILE__, __LINE__, __FUNCTION__, n);
  }
}

}  // namespace core

