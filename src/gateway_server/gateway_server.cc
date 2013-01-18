//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-10 20:18:53.
// File name: gateway_server.cc
//
// Description: 
// Define class GatewayServer.
//

#include "gateway_server/gateway_server.h"

#include <boost/bind.hpp>

#include "common/server_type_types.h"
#include "core/base/logging.h"
#include "data_proxy_server/client/data_proxy_client_session.h"
#include "protocol/protocol_keyvalue_message_types.h"
#include "gateway_server/http_context.h"
#include "global/async_logging_singleton.h"
#include "global/time_consuming.h"

namespace squirrel {

using namespace global;

namespace gateway {

GatewayServer::GatewayServer(const core::InetAddress &address,
    const std::string &name) : server_(address, name) {}

GatewayServer::~GatewayServer() {}

bool GatewayServer::Initialize(const ServerConfigure &configure, const std::string &log) {
  if(this->StartAsyncLogging(configure, log) == false) {
    CoreLog(ERROR, "%s:%d (%s) [GatewayServer] StartAsyncLogging failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  this->id_ = configure.id_;

  this->thread_pool_.reset(new (std::nothrow) MessageThreadPool());
  if(this->thread_pool_ == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [GatewayServer] Allocate thread pool error.",
        __FILE__, __LINE__, __FUNCTION__);
    return false; 
  }

  if(this->thread_pool_->Initialize((size_t)configure.thread_number_, "GatewayThread", true) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [GatewayServer] Initialize thread pool error.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }


  // Schedule client.
  if(this->schedule_client_.Initialize(this->thread_pool_->GetNextLoop(),
        this->GetServerID(), common::ServerType::SERVER_TYPE_GATEWAY, 
        core::InetAddress(configure.ip_, configure.port_), configure.default_schedule_id_,
        core::InetAddress(configure.default_schedule_ip_, configure.default_schedule_port_)) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [GatewayServer] Initialize tcp schedule client error.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  this->schedule_client_.SetJoinScheduleCallback(boost::bind(&GatewayServer::OnAppServerJoin, this, _1));
  this->schedule_client_.SetDisjoinScheduleCallback(boost::bind(&GatewayServer::OnAppServerDisjoin, this, _1));
  schedule::client::ScheduleClientSingleton::SetScheduleClient(&this->schedule_client_);


  // Data proxy client.
  if(this->data_proxy_client_.Initialize(this->thread_pool_->GetNextLoop(),
        this->GetServerID(), common::ServerType::SERVER_TYPE_GATEWAY,
        core::InetAddress(configure.ip_, configure.port_)) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [GatewayServer] Initialize tcp data proxy client error.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  dataproxy::client::DataProxyClientSingleton::SetDataProxyClient(&this->data_proxy_client_);
  this->data_proxy_client_.SetResponseCallback(boost::bind(&DataRequestHandler::OnResponse,
        &data_request_handler_, _1));


  if(this->server_.Initialize(this->thread_pool_->GetNextLoop(),
        this->thread_pool_->GetEventLoopThreadGroup()) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [GatewayServer] Initialize tcp server error.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  this->http_service_handler_.SetHttpCallback(boost::bind(&GatewayServer::OnHttpRequest, this, _1));

  this->server_.SetConnectionCallback(boost::bind(&HttpServiceHandler::OnConnection,
        &this->http_service_handler_, _1));
  this->server_.SetMessageCallback(boost::bind(&HttpServiceHandler::OnMessage,
        &this->http_service_handler_, _1, _2, _3));
  this->server_.SetWriteCallback(boost::bind(&HttpServiceHandler::OnWriteComplete,
        &this->http_service_handler_, _1));

  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[GatewayServer] Gateway server initialize complete.");

  return true;
}

void GatewayServer::Start() {
  this->thread_pool_->Start();
  this->server_.Start();
  this->schedule_client_.Start();
  this->data_proxy_client_.Start();
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[GatewayServer] Gateway server start complete.");
}

void GatewayServer::Stop() {
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[GatewayServer] Gateway server stop.");
  this->server_.Stop();
  this->thread_pool_->Stop();
  this->schedule_client_.Stop();
  this->data_proxy_client_.Stop();
  AsyncLoggingSingleton::GetAsyncLogging()->Stop();
}

bool GatewayServer::StartAsyncLogging(const ServerConfigure &configure, const std::string &log) {
  AsyncLogging *logging = AsyncLoggingSingleton::GetAsyncLogging();
  if(logging && logging->Initialize(configure.log_ + log, kMaxLoggingSize)) {
    CoreLog(INFO, "%s:%d (%s) [GatewayServer] Start server log [%s].",
        __FILE__, __LINE__, __FUNCTION__, (configure.log_ + log).c_str());
    logging->Start();
    return true;
  }

  return false;
}

bool GatewayServer::OnHttpRequest(const core::TCPConnectionPtr &connection) {
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
      "[GatewayServer] Receive request from http client.");
#ifdef _TIME_CONSUMING_TEST
  global::TimeConsuming time_consume("GatewayServer::OnHttpRequest");
#endif
  HttpContext *context = &boost::any_cast<HttpContext &>(connection->GetContext());
  if(context == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [GatewayServer] OnHttpRequest get http context failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  if(context->GetRequestID() != 0) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [GatewayServer] OnHttpRequest GetRequestID is not 0.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  // Each EventLoop(thread) has a independent packet tool, so it is thread-safe.
  const core::EventLoop *loop = connection->GetLoop();
  global::ThriftPacketPtr packet_ptr = this->thread_pool_->GetPacket(loop);
  if(packet_ptr == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [GatewayServer] Get thrift packet failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  protocol::Request request;
  if(packet_ptr->Deserialize<protocol::Request>(&request,
        context->GetRequest().GetContent()) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [GatewayServer] Deserialize Request[%ld] failed.",
        __FILE__, __LINE__, __FUNCTION__, context->GetRequest().GetContent().size());
    return false;
  }
  return this->data_request_handler_.OnRequest(request, packet_ptr, connection);
}

void GatewayServer::OnAppServerJoin(const common::MessageScheduleLoginSync *message) {
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[GatewayServer] Appserver login [id=%d, type=%d, host=%s, port=%d].",
      message->id_, message->type_, message->host_.c_str(), message->port_);
  if(message->type_ == common::ServerType::SERVER_TYPE_DATA_PROXY) {
    dataproxy::client::DataProxyClientSessionPtr session (new (std::nothrow)
        dataproxy::client::DataProxyClientSession(core::InetAddress(message->host_,
            message->port_), "DataProxyClientSession"));
    if(session == NULL) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [GatewayServer] Allocate DataProxyClientSession error.",
          __FILE__, __LINE__, __FUNCTION__);
      return ;
    }
    if(session->Initialize(this->thread_pool_->GetNextLoop(), message->id_) == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [GatewayServer] Initialize DataProxyClientSession error.",
          __FILE__, __LINE__, __FUNCTION__);
      return ;
    }
    if(session->Start() == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [GatewayServer] Start DataProxyClientSession error.",
          __FILE__, __LINE__, __FUNCTION__);
      return ;
    }
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
        "[GatewayServer] DataProxyClientSession[%d] start.", message->id_);
  }
}

void GatewayServer::OnAppServerDisjoin(const common::MessageScheduleLogoutSync *message) {
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[GatewayServer] Appserver logout [id=%d, type=%d].", message->id_, message->type_);
  if(message->type_ == common::ServerType::SERVER_TYPE_DATA_PROXY) {
    dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->RemoveSession(message->id_);
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
        "[GatewayServer] DataProxyClientSession[%d] stop.", message->id_);
  }
}

}  // namespace gateway

}  // namespace squirrel

