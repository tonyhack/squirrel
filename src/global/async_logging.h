//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-03 13:43:52.
// File name: async_logging.h
//
// Description: 
// Define class AsyncLogging.
//

#ifndef _squirrel_global__ASYNC__LOGGING__H
#define _squirrel_global__ASYNC__LOGGING__H

#include <boost/shared_ptr.hpp>

#include "core/loop_thread.h"
#include "core/base/noncopyable.h"
#include "global/async_logging_loop.h"
#include "global/logging_level.h"

namespace squirrel {

namespace global {

class AsyncLogging : public core::Noncopyable {
  typedef boost::shared_ptr<core::LoopThread> LoopThreadPtr;
 public:
  AsyncLogging();
  ~AsyncLogging();

  bool Initialize(const std::string file, int max_logging_size);
  inline void SetLoggingLevel(LoggingLevel level) {
    this->logging_level_ = level;
  }

  void Logging(LoggingLevel level, const char *log, ...);

  void Start();
  void Stop();

 private:
  AsyncLoggingLoop loop_;
  LoopThreadPtr loop_thread_;

  LoggingLevel logging_level_;
};

}  // namespace global

}  // namespace squirrel

#endif  // _squirrel_global__ASYNC__LOGGING__H

