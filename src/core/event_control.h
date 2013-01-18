//
// Summary: server core source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-08-27 15:42:58.
// File name: event_control.h
//
// Description: 
// Define class EventControl.
//

#ifndef __CORE__EVENT__CONTROL__H
#define __CORE__EVENT__CONTROL__H

#include "core/base/noncopyable.h"
#include "core/base/time_tick.h"

namespace core {

class Channel;
class EventLoop;
class EventHandler;

class EventControl : public Noncopyable {
 public:
  explicit EventControl(EventLoop *loop);
  ~EventControl();

  bool Initialize();
  bool Start();

  void SetEvent(const EventHandler *handler);
  void OnReadCallback(const TimeTick &time_tick);

 private:
  Channel *channel_;
  EventLoop *loop_;
  int fd_[2];
};

};

#endif  // __CORE__EVENT__CONTROL__H

