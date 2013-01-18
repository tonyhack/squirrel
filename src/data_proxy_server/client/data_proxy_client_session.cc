//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-10 13:20:23.
// File name: data_proxy_client_session.cc
//
// Description: 
// Define class DataProxyClientSession.
//

#include "data_proxy_server/client/data_proxy_client_session.h"

#include <boost/bind.hpp>

#include "common/message_data_proxy_types.h"
#include "common/message_types.h"
#include "common/protocol.h"
#include "data_proxy_server/client/data_proxy_client.h"

namespace squirrel {

namespace dataproxy {

namespace client {

DataProxyClientSession::DataProxyClientSession(const core::InetAddress &server_address,
    const std::string &name) : server_id_(0), tcp_client_(server_address, name),
    loop_(NULL), login_server_(false) {}

DataProxyClientSession::~DataProxyClientSession() {}

bool DataProxyClientSession::Initialize(core::EventLoop *loop, core::uint32 server_id) {
  assert(loop);
  if(this->tcp_client_.Initialize(loop) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [DataProxyClientSession] TCPClient initialize failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  this->server_id_ = server_id;

  this->tcp_client_.SetConnectionCallback(boost::bind(
        &DataProxyClientSession::OnConnection, this, _1));
  this->tcp_client_.SetMessageCallback(boost::bind(
        &DataProxyClientSession::OnMessage, this, _1, _2, _3));
  this->tcp_client_.SetWriteCallback(boost::bind(
        &DataProxyClientSession::OnWriteComplete, this, _1));

  return true;
}

bool DataProxyClientSession::Start() {
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[DataProxyClientSession] start to connect [%s].",
      this->tcp_client_.GetServerAddress().ToString().c_str());
  if(DataProxyClientSingleton::GetDataProxyClient()->AddSession(
        this->shared_from_this()) == true) {
    this->tcp_client_.Start();
    return true;
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [DataProxyClientSession] AddSession failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
}

void DataProxyClientSession::Stop() {
  this->login_server_ = false;
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[DataProxyClientSession] stopped connection [%s].",
      this->tcp_client_.GetServerAddress().ToString().c_str());
  this->tcp_client_.Stop();
  this->connection_.reset();
}

void DataProxyClientSession::SendMessage(const char *message, size_t size) {
  assert(this->connection_);
  common::ProtocolHead head = size;
  {
    core::AutoMutex auto_mutex(&this->mutex_);
    this->mt_buffer_.PreWrite(sizeof(common::kProtocolHeadSize) + size);
    memcpy(this->mt_buffer_.HeadPointer(), &head, sizeof(common::ProtocolHead));
    memcpy(this->mt_buffer_.RandomPointer(sizeof(common::ProtocolHead)), message, size);
  }
  this->connection_->SendMessage(this->mt_buffer_.HeadPointer(),
      sizeof(common::kProtocolHeadSize) + size);
}

void DataProxyClientSession::Send(const char *message, size_t size) {
  assert(this->connection_);
  common::ProtocolHead head = size;
  this->buffer_.PreWrite(sizeof(common::kProtocolHeadSize) + size);
  memcpy(this->buffer_.HeadPointer(), &head, sizeof(common::ProtocolHead));
  memcpy(this->buffer_.RandomPointer(sizeof(common::ProtocolHead)), message, size);
  this->connection_->SendMessage(this->buffer_.HeadPointer(),
      sizeof(common::kProtocolHeadSize) + size);
}

void DataProxyClientSession::Send(const common::MessageDataProxy *message) {
  size_t size = 0;
  const char *msg_ptr = this->packet_.Serialize<common::MessageDataProxy>(message, size);
  if(msg_ptr) {
    common::Message msg;
    msg.__set_type_(common::MessageType::MESSAGE_TYPE_DATA_PROXY);
    msg.message_.assign(msg_ptr, size);
    msg_ptr = this->packet_.Serialize<common::Message>(&msg, size);
    if(msg_ptr) {
      this->Send(msg_ptr, size);
    } else {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [DataProxyClientSession] Serialize MessageDataProxy failed.",
          __FILE__, __LINE__, __FUNCTION__);
    }
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [DataProxyClientSession] Serialize MessageDataProxy failed.",
        __FILE__, __LINE__, __FUNCTION__);
  }
}

void DataProxyClientSession::OnConnection(const core::TCPConnectionPtr &connection) {
  if(connection->CheckConnected()) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
        "[DataProxyClientSession] connection [%s] established.",
        connection->GetPeerAddress().ToString().c_str());
    this->connection_ = connection;
    this->LoginServer();
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
        "[DataProxyClientSession] connection [%s] disconnected.",
        connection->GetPeerAddress().ToString().c_str());
    this->login_server_ = false;
    DataProxyClientSingleton::GetDataProxyClient()->RemoveSession(this->GetServerID());
    this->connection_.reset();
  }
}

void DataProxyClientSession::OnMessage(const core::TCPConnectionPtr &connection,
    core::ByteBufferDynamic &buffer, const core::TimeTick &time_tick) {
  while(true) {
    size_t read_size = buffer.ReadSize();
    if(read_size < common::kProtocolHeadSize) {
      break ;
    }
    size_t message_size = common::GetProtocolSize(buffer.ReadPointer());
    if(read_size < message_size) {
      break ;
    }
    // Deserialize Message.
    common::Message message;
    if(this->packet_.Deserialize<common::Message>(&message,
          common::GetProtocolData(buffer.ReadPointer()),
          message_size - common::kProtocolHeadSize) == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [DataProxyClientSession] Deserialize Message failed.",
          __FILE__, __LINE__, __FUNCTION__);
      break ;
    }
    // Shift read pointer.
    buffer.ReadShift(message_size);
    // Deserialize MessageDataProxy.
    if(message.type_ != common::MessageType::MESSAGE_TYPE_DATA_PROXY) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [DataProxyClientSession] Message type error, is not MESSAGE_TYPE_DATA_PROXY.",
          __FILE__, __LINE__, __FUNCTION__);
      break ;
    }
    common::MessageDataProxy message_data_proxy;
    if(this->packet_.Deserialize<common::MessageDataProxy>(&message_data_proxy,
          message.message_.data(), message.message_.size()) == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [DataProxyClientSession] Deserialize MessageDataProxy failed.",
          __FILE__, __LINE__, __FUNCTION__);
      break ;
    }
    // Handle message.
    this->DispatchMessage(&message_data_proxy);
  }
}

void DataProxyClientSession::OnWriteComplete(const core::TCPConnectionPtr &connection) {}

void DataProxyClientSession::LoginServer() {
  // Send login request to dataproxy server.
  common::MessageDataProxyLoginReq message;
  message.__set_type_(DataProxyClientSingleton::GetDataProxyClient()->GetHostType());
  message.__set_id_(DataProxyClientSingleton::GetDataProxyClient()->GetHostID());
  message.__set_host_(DataProxyClientSingleton::GetDataProxyClient()->GetHostIP());
  message.__set_port_(DataProxyClientSingleton::GetDataProxyClient()->GetHostPort());
  this->Send<common::MessageDataProxyLoginReq>(&message,
      common::MessageDataProxyType::MESSAGE_DATA_PROXY_TYPE_LOGIN_REQ);
  // Log.
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[DataProxyClientSession] Send MESSAGE_DATA_PROXY_TYPE_LOGIN_REQ to [%s].",
      this->tcp_client_.GetServerAddress().ToString().c_str());
}

void DataProxyClientSession::DispatchMessage(const common::MessageDataProxy *message) {
  switch(message->type_) {
    case common::MessageDataProxyType::MESSAGE_DATA_PROXY_TYPE_LOGIN_RES:
      this->OnLoginServerResponse(message->message_.data(), message->message_.size());
      break;

    case common::MessageDataProxyType::MESSAGE_DATA_PROXY_TYPE_LOGOUT_RES:
      this->OnLogoutServerResponse(message->message_.data(), message->message_.size());
      break;

    case common::MessageDataProxyType::MESSAGE_DATA_PROXY_TYPE_DATA_RESPONSE:
      this->OnResponse(message->message_.data(), message->message_.size());
      break;

    default:
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [DataProxyClientSession] receive message type is invalid.",
          __FILE__, __LINE__, __FUNCTION__);
      break;
  }
}

void DataProxyClientSession::OnLoginServerResponse(const char *message, size_t size) {
  common::MessageDataProxyLoginRes res;
  // Deserialize.
  if(this->packet_.Deserialize<common::MessageDataProxyLoginRes>(&res, message, size) == true) {
    if(res.result_ == true) {
      this->login_server_ = true;
    } else {
      this->login_server_ = false;
      this->Stop();
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR, 
          "%s:%d (%s) [DataProxyClientSession] login dataproxy repsonsed, the result is failed.",
          __FILE__, __LINE__, __FUNCTION__);
    }
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR, 
        "%s:%d (%s) [DataProxyClientSession] Deserialize MessageDataProxyLoginRes failed.",
        __FILE__, __LINE__, __FUNCTION__);
  }
}

void DataProxyClientSession::OnLogoutServerResponse(const char *message, size_t size) {
  common::MessageDataProxyLogoutRes res;
  // Deserialize
  if(this->packet_.Deserialize<common::MessageDataProxyLogoutRes>(&res, message, size) == true) {
    if(res.result_ == true) {
      this->login_server_ = false;
    } else {
      this->login_server_ = true;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR, 
          "%s:%d (%s) [DataProxyClientSession] logout dataproxy repsonsed, the result is failed.",
          __FILE__, __LINE__, __FUNCTION__);
    }
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR, 
        "%s:%d (%s) [DataProxyClientSession] Deserialize MessageDataProxyLogoutRes failed.",
        __FILE__, __LINE__, __FUNCTION__);
  }
}

void DataProxyClientSession::OnResponse(const char *message, size_t size) {
  common::MessageDataProxyDataResponse response;
  // Deserialize.
  if(this->packet_.Deserialize<common::MessageDataProxyDataResponse>(
        &response, message, size) == true) {
    DataProxyClientSingleton::GetDataProxyClient()->OnResponse(&response);
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR, 
        "%s:%d (%s) [DataProxyClientSession] Deserialize MessageDataProxyDataResponse failed.",
        __FILE__, __LINE__, __FUNCTION__);
  }
}

}  // namespace client

}  // namespace dataproxy

}  // namespace squirrel

