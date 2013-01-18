//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-03 17:56:38.
// File name: data_proxy_server.cc
//
// Description: 
// Define class DataProxyServer.
//

#include "data_proxy_server/server/data_proxy_server.h"

#include <boost/bind.hpp>

#include "common/message_types.h"
#include "common/protocol.h"
#include "common/server_type_types.h"
#include "core/base/logging.h"
#include "global/async_logging_singleton.h"

namespace squirrel {

namespace dataproxy {

namespace server {

namespace {

struct RequestTerminalContext {
  RequestTerminalContext(RequestTerminal *terminal)
    : terminal_(terminal) {}
  RequestTerminalPtr terminal_;
};

}  // namespace

DataProxyServer::DataProxyServer(const core::InetAddress &address,
    const std::string &name) : server_(address, name) {}

DataProxyServer::~DataProxyServer() {}

bool DataProxyServer::Initialize(const ServerConfigure &configure,
    const std::string &log) {
  if(this->StartAsyncLogging(configure, log) == false) {
    CoreLog(core::ERROR, "%s:%d (%s) [DataProxyServer] StartAsyncLogging failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  this->server_id_ = configure.id_;

  this->thread_pool_.reset(new (std::nothrow) core::EventLoopThreadGroup());
  if(this->thread_pool_ == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [DataProxyServer] Allocate thread pool error.",
        __FILE__, __LINE__, __FUNCTION__);
    return false; 
  }

  if(this->thread_pool_->Initialize((size_t)configure.message_thread_number_,
        "DataProxyThread", true) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [DataProxyServer] Initialize thread pool error.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  if(this->schedule_client_.Initialize(this->thread_pool_->GetNextLoop(),
        this->GetServerID(), common::ServerType::SERVER_TYPE_DATA_PROXY, 
        core::InetAddress(configure.ip_, configure.port_),
        configure.default_schedule_id_, core::InetAddress(configure.default_schedule_ip_,
          configure.default_schedule_port_)) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [DataProxyServer] Initialize tcp schedule client error.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  this->schedule_client_.SetJoinScheduleCallback(
      boost::bind(&DataProxyServer::OnAppServerJoin, this, _1));
  this->schedule_client_.SetDisjoinScheduleCallback(
      boost::bind(&DataProxyServer::OnAppServerDisjoin, this, _1));

  schedule::client::ScheduleClientSingleton::SetScheduleClient(&this->schedule_client_);

  if(this->server_.Initialize(this->thread_pool_->GetNextLoop(),
        this->thread_pool_.get()) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [DataProxyServer] Initialize tcp server error.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  this->server_.SetConnectionCallback(boost::bind(&DataProxyServer::OnConnection,
        this, _1));
  this->server_.SetMessageCallback(boost::bind(&DataProxyServer::OnMessage,
        this, _1, _2, _3));
  this->server_.SetWriteCallback(boost::bind(&DataProxyServer::OnWriteComplete,
        this, _1));

  if(this->request_service_.Initialize(configure.max_buffer_size_,
        configure.read_thread_number_, configure.write_thread_number_,
        configure.redis_configure_, configure.storage_configure_,
        configure.procedure_configure_) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [DataProxyServer] Initialize request service error.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  RequestServiceSingleton::SetRequestService(&this->request_service_);

  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[DataProxyServer] Dataproxy server initialize complete.");

  return true;
}

void DataProxyServer::Start() {
  this->request_service_.Start();
  this->thread_pool_->Start();
  this->server_.Start();
  this->schedule_client_.Start();
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[DataProxyServer] DataProxyServer start complete.");
}

void DataProxyServer::Stop() {
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[DataProxyServer] DataProxyServer stoped.");
  this->schedule_client_.Stop();
  this->server_.Stop();
  this->thread_pool_->Stop();
  this->request_service_.Stop();
  AsyncLoggingSingleton::GetAsyncLogging()->Stop();
}

bool DataProxyServer::StartAsyncLogging(const ServerConfigure &configure,
    const std::string &log) {
  AsyncLogging *logging = AsyncLoggingSingleton::GetAsyncLogging();
  if(logging && logging->Initialize(configure.log_ + log, kMaxLoggingSize)) {
    core::CoreLog(core::INFO, "%s:%d (%s) [DataProxyServer] Start server log [%s].",
        __FILE__, __LINE__, __FUNCTION__, (configure.log_ + log).c_str());
    logging->Start();
    return true;
  }

  return false;
}

void DataProxyServer::OnConnection(const core::TCPConnectionPtr &connection) {
  if(connection->CheckConnected()) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
        "[DataProxyServer] Connection established.");
    RequestTerminal *request_terminal = new (std::nothrow) RequestTerminal();
    if(request_terminal) {
      request_terminal->Initialize(connection);
      RequestTerminalContext context(request_terminal);
      connection->GetContext() = context;
    } else {
      connection->Shutdown();
    }
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
        "[DataProxyServer] Connection disconnected.");
    RequestTerminalContext *context =
      &boost::any_cast<RequestTerminalContext &>(connection->GetContext());
    if(context) {
      context->terminal_->OnDisConnnect();
      connection->ClearContext();
    }
  }
}

void DataProxyServer::OnMessage(const core::TCPConnectionPtr &connection,
    core::ByteBufferDynamic &buffer, const core::TimeTick &time_tick) {
  while(true) {
    size_t read_size = buffer.ReadSize();
    // Can read a message head.
    if(read_size < common::kProtocolHeadSize) {
      break;
    }
    size_t message_size = common::GetProtocolSize(buffer.ReadPointer());
    // Can read a whole message.
    if(read_size < message_size) {
      break;
    }
    // Deserialize Message.
    common::Message message;
    if(this->packet_.Deserialize<common::Message>(&message,
          common::GetProtocolData(buffer.ReadPointer()),
          message_size - common::kProtocolHeadSize) == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [DataProxyServer] Deserialize Message failed.",
          __FILE__, __LINE__, __FUNCTION__);
      break;
    }
    // Deserialize MessageDataProxy.
    if(message.type_ != common::MessageType::MESSAGE_TYPE_DATA_PROXY) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [DataProxyServer] Message type[%d] error, is not MESSAGE_TYPE_DATA_PROXY.",
          __FILE__, __LINE__, __FUNCTION__, message.type_);
      break;
    }
    common::MessageDataProxy message_dataproxy;
    if(this->packet_.Deserialize<common::MessageDataProxy>(&message_dataproxy,
          message.message_.data(), message.message_.size()) == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [DataProxyServer] Deserialize MessageDataProxy failed.",
          __FILE__, __LINE__, __FUNCTION__);
      break;
    }
    // Get ScheduleTerminal.
    assert(connection);
    if(connection->GetContext().empty() == true) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [DataProxyServer] Connection's context is empty.",
          __FILE__, __LINE__, __FUNCTION__);
      break;
    }
    RequestTerminalContext *context =
      &boost::any_cast<RequestTerminalContext &>(connection->GetContext());
    assert(context->terminal_);
    // OnMessage.
    context->terminal_->OnMessage(&message_dataproxy);
    // Shift read pointer.
    buffer.ReadShift(message_size);
  }
}

void DataProxyServer::OnWriteComplete(const core::TCPConnectionPtr &connection) {
#ifdef _TIME_CONSUMING_TEST
  RequestServiceSingleton::GetRequestService()->request_consume_.ConsumeLogging();
#endif
}

void DataProxyServer::OnAppServerJoin(const common::MessageScheduleLoginSync *message) {
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[DataProxyServer] Appserver login [id=%d, type=%d, host=%s, port=%d].",
      message->id_, message->type_, message->host_.c_str(), message->port_);
}

void DataProxyServer::OnAppServerDisjoin(const common::MessageScheduleLogoutSync *message) {
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[GatewayServer] Appserver logout [id=%d, type=%d].",
      message->id_, message->type_);
}

}  // namespace server

}  // namespace dataproxy

}  // namespace squirrel

