//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-16 16:15:25.
// File name: time_consuming.h
//
// Description: 
// Define class TimeConsuming.
//

#ifndef _squirrel_global__TIME__CONSUMING__H
#define _squirrel_global__TIME__CONSUMING__H

#include <string>

#include "core/base/time_tick.h"
#include "global/async_logging_singleton.h"

namespace squirrel {

namespace global {

class TimeConsuming {
 public:
  TimeConsuming() {
    this->timestamp_.Stamp();
  }
  TimeConsuming(const std::string &debug) {
    this->timestamp_.Stamp();
    this->debug_ = debug;
  }
  explicit TimeConsuming(const TimeConsuming &time) {
    this->timestamp_ = time.timestamp_;
    this->debug_ = time.debug_;
  }

  ~TimeConsuming() {
    core::TimestampNanosecond now;
    now.Stamp();
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
        "[TimeConsuming] ~TimeConsuming debug[%s] now[%ld] cost [%ld] ns.",
        this->debug_.c_str(), now.GetStamp(), now.AbsoluteDistance(this->timestamp_));
  }

  inline void ConsumeLogging() {
    core::TimestampNanosecond now;
    now.Stamp();
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
        "[TimeConsuming] ConsumeLogging debug[%s] now[%ld] cost [%ld] ns.",
        this->debug_.c_str(), now.GetStamp(), now.AbsoluteDistance(this->timestamp_));
  }

  inline void ResetConsuming() {
    this->timestamp_.Stamp();
  }
  inline void SetDebugInfo(const std::string &debug) {
    this->debug_ = debug;
  }
  inline void SetTimestamp(const core::TimestampNanosecond &timestamp) {
    this->timestamp_ = timestamp;
  }
  inline core::TimestampNanosecond GetTimestamp() const {
    return this->timestamp_;
  }

 private:
  core::TimestampNanosecond timestamp_;
  std::string debug_;
};

}  // namespace global

}  // namespace squirrel

#endif  // _squirrel_global__TIME__CONSUMING__H

