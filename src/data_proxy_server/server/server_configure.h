//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-03 17:22:23.
// File name: server_configure.h
//
// Description: 
// Define class ServerConfigure.
//

#ifndef _squirrel_dataproxy_server__SERVER__CONFIGURE__H
#define _squirrel_dataproxy_server__SERVER__CONFIGURE__H

#include <string>

#include "core/base/noncopyable.h"
#include "core/base/types.h"
#include "data_proxy_server/storage_proxy/procedure_configure.h"
#include "data_proxy_server/redis_proxy/redis_configure.h"
#include "data_proxy_server/storage_proxy/storage_configure.h"

namespace squirrel {

namespace dataproxy {

namespace server {

class ServerConfigure : public core::Noncopyable {
 public:
  ServerConfigure(core::uint32 id);
  ~ServerConfigure();

  bool Load(const char *file);

  core::uint32 id_;
  std::string ip_;
  core::uint16 port_;

  core::uint8 message_thread_number_;
  core::uint8 read_thread_number_;
  core::uint8 write_thread_number_;

  std::string log_;
  std::string core_configure_;
  std::string core_log_;

  core::uint32 default_schedule_id_;
  std::string default_schedule_ip_;
  core::uint16 default_schedule_port_;

  core::uint32 max_buffer_size_;

  redis::RedisConfigure redis_configure_;
  storage::StorageConfigure storage_configure_;
  storage::ProcedureConfigure procedure_configure_;
};

}  // namespace server

}  // namespace dataproxy

}  // namespace squirrel

#endif  // _squirrel_dataproxy_server__SERVER__CONFIGURE__H

