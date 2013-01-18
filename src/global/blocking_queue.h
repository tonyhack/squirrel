//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-08 20:34:32.
// File name: blocking_queue.h
//
// Description: 
// Define class BlockingQueue.
//

#include "core/base/noncopyable.h"

#ifndef _squirrel_global__BLOCKING__QUEUE__H
#define _squirrel_global__BLOCKING__QUEUE__H

#include <deque>

#include "core/base/auto_mutex.h"
#include "core/base/condition_variable.h"
#include "core/base/mutex.h"
#include "core/base/noncopyable.h"

namespace squirrel {

namespace global {

template <typename T>
class BlockingQueue : public core::Noncopyable {
  typedef std::deque<T> Queue;
 public:
  BlockingQueue() {}
  ~BlockingQueue() {}

  void Push(const T &e) {
    core::AutoMutex auto_mutex(&this->mutex_);
    this->queue_.push_back(e);
    this->condition_.Signal();
  }

  T Pop() {
    core::AutoMutex auto_mutex(&this->mutex_);
    while(this->queue_.empty()) {
      this->condition_.Wait(&this->mutex_);
    }
    T e = this->queue_.front();
    this->queue_.pop_front();
    return e;
  }

  size_t Size() {
    return this->queue_.size();
  }

 private:
  Queue queue_;

  core::Mutex mutex_;
  core::ConditionVariable condition_;
};

}  // namespace global

}  // namespace squirrel

#endif  // _squirrel_global__BLOCKING__QUEUE__H

