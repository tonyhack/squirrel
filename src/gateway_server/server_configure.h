//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-10 20:20:05.
// File name: server_configure.h
//
// Description: 
// Define class ServerConfigure.
//

#ifndef _squirrel_gateway__SERVER__CONFIGURE__H
#define _squirrel_gateway__SERVER__CONFIGURE__H

#include <string>

#include "core/base/noncopyable.h"
#include "core/base/types.h"

using namespace core;

namespace squirrel {

namespace gateway {

class ServerConfigure : public core::Noncopyable {
 public:
  ServerConfigure(uint32 id);
  ~ServerConfigure();

  bool Load(const char *file);

  uint32 id_;
  std::string ip_;
  uint16 port_;
  uint8 thread_number_;
  std::string log_;
  std::string core_log_;
  std::string core_configure_;

  uint32 default_schedule_id_;
  std::string default_schedule_ip_;
  uint16 default_schedule_port_;
};

}  // namespace gateway

}  // namespace squirrel

#endif  // _squirrel_gateway__SERVER__CONFIGURE__H

