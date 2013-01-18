//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-03 13:46:18.
// File name: async_logging_singleton.h
//
// Description: 
// Define class AsyncLoggingSingleton.
//

#ifndef _squirrel_global__ASYNC__LOGGING__SINGLETON__H
#define _squirrel_global__ASYNC__LOGGING__SINGLETON__H

#include "core/base/noncopyable.h"
#include "global/async_logging.h"

namespace squirrel {

namespace global {

static const int kMaxLoggingSize = 5 * 1024;

class AsyncLoggingSingleton : public core::Noncopyable {
 public:
  AsyncLoggingSingleton() {}
  ~AsyncLoggingSingleton() {}

  static inline AsyncLogging *GetAsyncLogging() {
    return &AsyncLoggingSingleton::async_logging_;
  }

 private:
  static AsyncLogging async_logging_;
};

}  // namespace squirrel

}  // namespace global

#endif  // _squirrel_global__ASYNC__LOGGING__SINGLETON__H

