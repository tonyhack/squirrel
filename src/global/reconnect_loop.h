//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-08 20:42:18.
// File name: reconnect_loop.h
//
// Description: 
// Define class ReconnectLoop.
//

#ifndef _squirrel_global__RECONNECT__LOOP__H
#define _squirrel_global__RECONNECT__LOOP__H

#include "core/base_loop.h"
#include "core/base/noncopyable.h"
#include "global/blocking_queue.h"

namespace squirrel {

namespace global {

template <typename T>
class ReconnectLoop : public core::BaseLoop {
 public:
  ReconnectLoop() {}
  virtual ~ReconnectLoop() {}

  virtual void Loop() {
    useconds_t max_usecs = 1000 * 1000;
    useconds_t min_usecs = 20 * 1000;
    useconds_t usecs = 0;

    this->looping_ = true;

    while(this->quit_ == false) {
      T element = this->queue_.Pop();
      // Check quit.
      if(element.CheckValid()) {
        if(element.CheckReconnect() == true &&
            element.Reconnect() == false) {
          // Reconnect failed, push it.
          this->queue_.Push(element);
          usecs = max_usecs / this->queue_.Size();
          if(usecs < min_usecs) usecs = min_usecs;
          usleep(usecs);
        }
      } else {
        this->quit_ = true;
      }
    }

    this->looping_ = false;
  }

  virtual void Wakeup() {
    T element;
    element.SetValid(true);
    element.SetReconnect(false);
    this->queue_.Push(element);
  }

  void Reconnect(const T &request) {
    this->queue_.Push(request);
  }

 private:
  global::BlockingQueue<T> queue_;
};

}  // namespace global

}  // namespace squirrel

#endif  // _squirrel_global__RECONNECT__LOOP__H

