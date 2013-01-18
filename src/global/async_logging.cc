//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-03 13:44:41.
// File name: async_logging.cc
//
// Description: 
// Define class AsyncLogging.
//

#include "global/async_logging.h"
#include "core/base/types.h"
#include "core/base/variable_parameter.h"

namespace squirrel {

namespace global {

static const core::uint32 kLoggingMaxSize = 9 * 1024;

static const int kFlushIntervalMsecs = 500;

AsyncLogging::AsyncLogging() : logging_level_(LOGGING_LEVEL_DEBUG) {}
AsyncLogging::~AsyncLogging() {}

bool AsyncLogging::Initialize(const std::string file, int max_logging_size) {
  if(this->loop_.Initialize(file, max_logging_size, kFlushIntervalMsecs) == false) {
    return false;
  } else {
    core::LoopThread *thread = new (std::nothrow) core::LoopThread(&this->loop_, "logging thread", true);
    if(thread == NULL) {
      return false;
    }
    this->loop_thread_.reset(thread);
  }
  return true;
}

void AsyncLogging::Logging(LoggingLevel level, const char *log, ...) {
  if(this->logging_level_ <= level) {
    char buffer[kLoggingMaxSize + 1] = {0};
    bzero(buffer, sizeof(buffer));
    GET_VARIABLE_PARAMETERS(buffer, sizeof(buffer), log);
    this->loop_.Logging(level, buffer);
  }
}

void AsyncLogging::Start() {
  this->loop_thread_->StartLoop();
}

void AsyncLogging::Stop() {
  this->loop_thread_->StopLoop(true);
}


}  // global

}  // namespace squirrel

