//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-10 20:18:17.
// File name: gateway_server.h
//
// Description: 
// Define class GatewayServer.
//

#ifndef _squirrel_gateway__GATEWAY__SERVER__H
#define _squirrel_gateway__GATEWAY__SERVER__H

#include <boost/shared_ptr.hpp>

#include "common/message_schedule_types.h"
#include "core/base/noncopyable.h"
#include "core/tcp_server.h"
#include "data_proxy_server/client/data_proxy_client.h"
#include "gateway_server/data_request_handler.h"
#include "gateway_server/message_thread_pool.h"
#include "gateway_server/server_configure.h"
#include "gateway_server/http_service_handler.h"
#include "schedule_server/client/schedule_client.h"

namespace squirrel {

namespace gateway {

class GatewayServer : public core::Noncopyable {
  typedef boost::shared_ptr<MessageThreadPool> MessageThreadPoolPtr;
 public:
  GatewayServer(const core::InetAddress &address, const std::string &name);
  ~GatewayServer();

  bool Initialize(const ServerConfigure &configure, const std::string &log);

  void Start();
  void Stop();

  inline core::uint32 GetServerID() const { return this->id_; }

 private:
  inline bool StartAsyncLogging(const ServerConfigure &configure, const std::string &log);

  bool OnHttpRequest(const core::TCPConnectionPtr &connection);

  void OnAppServerJoin(const common::MessageScheduleLoginSync *message);
  void OnAppServerDisjoin(const common::MessageScheduleLogoutSync *message);

  MessageThreadPoolPtr thread_pool_;
  core::TCPServer server_;

  HttpServiceHandler http_service_handler_;
  DataRequestHandler data_request_handler_;

  core::uint32 id_;

  schedule::client::ScheduleClient schedule_client_;
  dataproxy::client::DataProxyClient data_proxy_client_;
};

}  // namespace gateway

}  // namespace squirrel

#endif  // _squirrel_gateway__GATEWAY__SERVER__H

