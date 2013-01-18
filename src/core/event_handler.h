//
// Summary: server core source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-08-27 15:43:11.
// File name: event_handler.h
//
// Description: 
// Define class EventHandler.
//

#ifndef __CORE__EVENT__HANDLER__H
#define __CORE__EVENT__HANDLER__H

#include <boost/function.hpp>

#include "core/base/noncopyable.h"

namespace core {

typedef boost::function<void ()> EventCallback;

class EventHandler : public Noncopyable {
 public:
  EventHandler() {}
  ~EventHandler() {}

  inline void SetEventCallback(const EventCallback &callback) {
    this->event_callback_ = callback;
  }

  void OnEventCallback() {
    this->event_callback_();
  }

 private:
  EventCallback event_callback_;
};

}

#endif  // __CORE__EVENT__HANDLER__H

