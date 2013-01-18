//
// Summary: server core source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-08-27 15:40:14.
// File name: channel.h
//
// Description: 
// Define class Channel
// A channel describes a I/O event with a fd(file descriptor), this fd can be
// a socket, a timer fd or a signal fd.
//

#ifndef __CORE__CHANNEL__H
#define __CORE__CHANNEL__H

#include <string>

#include "core/callbacks.h"
#include "core/base/noncopyable.h"

namespace core {

class EventLoop;
class TimeTick;

class Channel : public Noncopyable {
 public:
  Channel(EventLoop *loop, int fd);
  ~Channel();

  inline bool CheckEventHanding() const { return this->event_handling_; }

  // Handle happend event.
  void HandleEvent(const TimeTick &timetick);

  // Set callback handle.
  inline void SetReadCallback(const ReadEventCallback &callback) {
    this->read_callback_ = callback;
  }
  inline void SetWriteCallback(const CommonEventCallback &callback) {
    this->write_callback_ = callback;
  }
  inline void SetCloseCallback(const CommonEventCallback &callback) {
    this->close_callback_ = callback;
  }
  inline void SetErrorCallback(const CommonEventCallback &callback) {
    this->error_callback_ = callback;
  }

  // Event.
  inline int GetEvent() const { return this->event_; }
  inline bool CheckNoneEvent() const { return this->event_ == kNoneEvent_; }
  inline bool CheckReadEvent() const { return this->event_ & kReadEvent_; }
  inline bool CheckWriteEvent() const { return this->event_ & kWriteEvent_; }
  // Enable event.
  inline void EnableRead() { this->event_ |= kReadEvent_; this->UpdateEvent(); }
  inline void EnableWrite() { this->event_ |= kWriteEvent_; this->UpdateEvent(); }
  inline void EnableAll() { this->event_ = kReadEvent_ | kWriteEvent_; this->UpdateEvent(); }
  // Disable event.
  inline void DisableAll() { this->event_ = kNoneEvent_; this->UpdateEvent(); }
  inline void DisableRead() { this->event_ &= ~kReadEvent_; this->UpdateEvent(); }
  inline void DisableWrite() { this->event_ &= ~kWriteEvent_; this->UpdateEvent(); }

  // In HandleEvent, invoke to get happened event.
  inline int GetHappenedEvent() const { return this->happened_event_; }
  // After epoll_wait, invoke to set happened event.
  inline void SetHappendEvent(int event) { this->happened_event_ = event; }

  inline int GetFd() const { return this->fd_; }
  inline void SetFd(int fd) { this->fd_ = fd; }
  inline EventLoop *GetEventLoop() const { return this->loop_; }
  inline void SetEventLoop(EventLoop *loop) { this->loop_ = loop; }

  // State.
  inline bool CheckStateInit() const { return this->state_ == kStateInit; }
  inline bool CheckStateAdded() const { return this->state_ == kStateAdded; }
  inline bool CheckStateDeleted() const { return this->state_ == kStateDeleted; }
  //
  inline void SetStateAdded() { this->state_ = kStateAdded; }
  inline void SetStateDeleted() { this->state_ = kStateDeleted; }

  // For debug
  std::string SerializeString() const;

 private:
  // Update event to epoller.
  void UpdateEvent();

  // Event type.
  static const int kReadEvent_;
  static const int kWriteEvent_;
  static const int kNoneEvent_;

  // Event file describes.
  int fd_;

  // Concerned event and happened event.
  int event_;
  int happened_event_;

  // State for epoller.
  int state_;

  // State type.
  static const int kStateInit;
  static const int kStateAdded;
  static const int kStateDeleted;

  // Read callback deliver a timetick parameter.
  // this will gives a TCP heartbeat time.
  ReadEventCallback read_callback_;
  CommonEventCallback write_callback_;
  CommonEventCallback close_callback_;
  CommonEventCallback error_callback_;

  // Running in this loop.
  EventLoop *loop_;

  bool event_handling_;
};

}  // namespace core

#endif  // __CORE__CHANNEL__H

