//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-03 13:45:01.
// File name: async_logging_loop.h
//
// Description: 
// Define class AsyncLoggingLoop.
//

#ifndef _squirrel_global__ASYNC__LOGGING__LOOP__H
#define _squirrel_global__ASYNC__LOGGING__LOOP__H

#include <boost/shared_ptr.hpp>

#include "core/base_loop.h"
#include "core/base/condition_variable.h"
#include "global/logging_level.h"

namespace core {

class ByteBufferDynamic;

}  // namespace core

namespace squirrel {

namespace global {

class AsyncLoggingLoop : public core::BaseLoop {
  typedef boost::shared_ptr<core::ByteBufferDynamic> ByteBufferDynamicPtr;
 public:
  AsyncLoggingLoop();
  virtual ~AsyncLoggingLoop();

  bool Initialize(const std::string file, int max_logging_size, int flush_interval_msecs = 5);

  virtual void Loop();
  virtual void Wakeup();

  void Logging(LoggingLevel level, const char *log);

 private:
  std::string file_;
  std::string file_time_;
  std::string file_suffix_;

  std::string temp_file_;

  ByteBufferDynamicPtr logging_buffer_;
  core::Mutex mutex_;
  core::ConditionVariable condition_;

  LoggingLevel level_;

  int flush_interval_msecs_;

  char buffer_[10240];
};

}  // namespace global

}  // namespace squirrel

#endif  // _squirrel_global__ASYNC__LOGGING__LOOP__H

