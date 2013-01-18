//
// Summary: server core source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-08-27 15:36:20.
// File name: base_loop.h
//
// Description: 
// Define interface Loop, it's the core of reactor and it will be
// ran in a thread.
//
//

#ifndef __CORE__BASE__LOOP__H
#define __CORE__BASE__LOOP__H

#include "core/base/noncopyable.h"

namespace core {

class BaseLoop : public Noncopyable {
 public:
  BaseLoop() : looping_(false), quit_(false) {}
  virtual ~BaseLoop() {}

  virtual void Loop() = 0;
  virtual bool Initialize(bool timer = true) { return true; }
  // virtual void Stop() = 0;

  inline bool GetLooping() const { return this->looping_; }
  inline void Quit() { this->quit_ = true; }
  virtual void Wakeup() = 0;

 protected:
  bool looping_;
  bool quit_;
};

}  // namespace core

#endif  // __CORE__BASE__LOOP__H

