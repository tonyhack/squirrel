//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-03 17:04:55.
// File name: data_proxy_server.h
//
// Description: 
// Define class DataProxyServer.
//

#ifndef _squirrel_dataproxy_server__DATA__PROXY__SERVER__H
#define _squirrel_dataproxy_server__DATA__PROXY__SERVER__H

#include <boost/shared_ptr.hpp>

#include "core/base/noncopyable.h"
#include "core/event_loop_thread_group.h"
#include "core/tcp_connection.h"
#include "core/tcp_server.h"
#include "data_proxy_server/server/request_service.h"
#include "data_proxy_server/server/server_configure.h"
#include "schedule_server/client/schedule_client.h"
#include "global/thrift_packet.h"

namespace squirrel {

namespace dataproxy {

namespace server {

class DataProxyServer : public core::Noncopyable {
  typedef boost::shared_ptr<core::EventLoopThreadGroup> EventLoopThreadGroupPtr;
 public:
  DataProxyServer(const core::InetAddress &address, const std::string &name);
  ~DataProxyServer();

  bool Initialize(const ServerConfigure &configure, const std::string &log);

  void Start();
  void Stop();

  inline core::uint32 GetServerID() const { return this->server_id_; }

  inline const core::InetAddress &GetLocalAddress() const {
    return this->server_.GetAddress();
  }

 private:
  inline bool StartAsyncLogging(const ServerConfigure &configure,
      const std::string &log);

  void OnConnection(const core::TCPConnectionPtr &connection);
  void OnMessage(const core::TCPConnectionPtr &connection,
      core::ByteBufferDynamic &buffer, const core::TimeTick &time_tick);
  void OnWriteComplete(const core::TCPConnectionPtr &connection);

  void OnAppServerJoin(const common::MessageScheduleLoginSync *message);
  void OnAppServerDisjoin(const common::MessageScheduleLogoutSync *message);

  core::uint32 server_id_;

  EventLoopThreadGroupPtr thread_pool_;
  core::TCPServer server_;

  schedule::client::ScheduleClient schedule_client_;

  RequestService request_service_;

  global::ThriftPacket packet_;
};

}  // namespace server

}  // namespace dataproxy

}  // namespace squirrel

#endif  // _squirrel_dataproxy_server__DATA__PROXY__SERVER__H

