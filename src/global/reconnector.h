//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-08 21:08:11.
// File name: reconnector.h
//
// Description: 
// Define class Reconnector.
//

#ifndef _squirrel_global__RECONNECTOR__H
#define _squirrel_global__RECONNECTOR__H

#include <boost/shared_ptr.hpp>
#include "core/loop_thread.h"
#include "core/base/noncopyable.h"
#include "global/reconnect_loop.h"

namespace squirrel {

namespace global {

template <typename T>
class Reconnector : public core::Noncopyable {
 public:
  Reconnector() : thread_(&loop_, "ReconnectorThread", true) {}
  ~Reconnector() {}

  void Start() {
    this->thread_.StartLoop();
  }

  void Stop() {
    this->thread_.StopLoop(true);
  }

  void Reconnect(const T &request) {
    this->loop_.Reconnect(request);
  }

 private:
  ReconnectLoop<T> loop_;
  core::LoopThread thread_;
};

}  // namespace global

}  // namespace squirrel

#endif  // _squirrel_global__RECONNECTOR__H

