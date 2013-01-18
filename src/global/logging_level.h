//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-03 13:46:56.
// File name: logging_level.h
//
// Description: 
// Define logging level
//

#ifndef _squirrel_global__LOGGING__LEVEL__H
#define _squirrel_global__LOGGING__LEVEL__H

namespace squirrel {

namespace global {

enum LoggingLevel {
  LOGGING_LEVEL_DEBUG = 0,
  LOGGING_LEVEL_WARNING,
  LOGGING_LEVEL_INFO,
  LOGGING_LEVEL_ERROR,
  LOGGING_LEVEL_FATAL,
  LOGGING_LEVEL_MAX,
};

static const std::string kLoggingLevelString[LOGGING_LEVEL_MAX] = {
  "DEBUG", "WARN", "INFO", "ERROR", "FATAL"
};

}  // namespace global

}  // namespace squirrel

#endif  // _squirrel_global__LOGGING__LEVEL__H

