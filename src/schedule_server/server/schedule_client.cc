//
// Summary: server core source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-08-27 18:36:11.
// File name: schedule_client.cc
//
// Description: 
// Define class ScheduleClient.
//

#include "schedule_server/server/schedule_client.h"

#include <boost/bind.hpp>

#include "core/tcp_client.h"
#include "core/tcp_connection.h"
#include "core/base/noncopyable.h"
#include "common/message_types.h"
#include "common/protocol.h"
#include "global/thrift_packet.h"
#include "schedule_server/server/schedule_server.h"

namespace squirrel {

namespace schedule {

namespace server {

ScheduleClient::ScheduleClient(const InetAddress &server_address,
    const std::string &name) : tcp_client_(server_address, name),
  loop_(NULL), login_schedule_(false) {}

ScheduleClient::~ScheduleClient() {}

bool ScheduleClient::Initialize(core::EventLoop *loop) {
  assert(loop);
  if(this->tcp_client_.Initialize(loop) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
    "%s:%d (%s) [ScheduleClient] TCPClient initialize failed.", __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  this->tcp_client_.SetConnectionCallback(boost::bind(
        &ScheduleClient::OnConnection, this, _1));
  this->tcp_client_.SetMessageCallback(boost::bind(
        &ScheduleClient::OnMessage, this, _1, _2, _3));
  this->tcp_client_.SetWriteCallback(boost::bind(
        &ScheduleClient::OnWriteComplete, this, _1));

  return true;
}

void ScheduleClient::Start() {
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[ScheduleClient] ScheduleClient start to connect [%s].",
      this->tcp_client_.GetServerAddress().ToString().c_str());
  this->tcp_client_.Start();
}
void ScheduleClient::Stop() {
  this->LogoutScheduleServer();
  this->login_schedule_ = false;
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[ScheduleClient] ScheduleClient stopped connection [%s].",
      this->tcp_client_.GetServerAddress().ToString().c_str());
  this->tcp_client_.Stop();
  this->connection_.reset();
}

void ScheduleClient::Send(const char *message, size_t size) {
  assert(this->connection_);
  common::ProtocolHead head = size;
/*
  this->buffer_.PreWrite(sizeof(common::kProtocolHeadSize) + size);
  memcpy(this->buffer_.HeadPointer(), &head, sizeof(common::ProtocolHead));
  memcpy(this->buffer_.RandomPointer(sizeof(common::ProtocolHead)), message, size);
  this->connection_->SendMessage(this->buffer_.HeadPointer(),
      sizeof(common::kProtocolHeadSize) + size);
*/
  this->connection_->SendMessage((const char *)&head, sizeof(common::ProtocolHead));
  this->connection_->SendMessage(message, size);
}

void ScheduleClient::Send(const MessageSchedule *message_schedule) {
  size_t size = 0;
  const char *msg_ptr = this->packet_.Serialize<common::MessageSchedule>(message_schedule, size);
  if(msg_ptr) {
    common::Message message;
    message.__set_type_(common::MessageType::MESSAGE_TYPE_SCHEDULE);
    message.message_.assign(msg_ptr, size);
    msg_ptr = this->packet_.Serialize<common::Message>(&message, size);
    if(msg_ptr) {
      this->Send(msg_ptr, size);
    } else {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [ScheduleClient] Serialize MessageSchedule failed.",
          __FILE__, __LINE__, __FUNCTION__);
    }
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [ScheduleClient] Serialize Message failed.", __FILE__, __LINE__, __FUNCTION__);
  }
}

void ScheduleClient::OnConnection(const core::TCPConnectionPtr &connection) {
  if(connection->CheckConnected()) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
        "[ScheduleClient] Schedule client's connection [%s] established.",
        connection->GetPeerAddress().ToString().c_str());
    this->connection_ = connection;
    this->LoginScheduleServer();
    if(this->connection_callback_)
      this->connection_callback_(connection);
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
        "[ScheduleClient] Schedule client's connection [%s] disconnected.",
        connection->GetPeerAddress().ToString().c_str());
    this->login_schedule_ = false;
    if(this->connection_callback_) {
      this->connection_callback_(connection);
    } else {
      this->LogoutScheduleServer();
      this->connection_.reset();
    }
  }
}

void ScheduleClient::OnMessage(const core::TCPConnectionPtr &connection,
    core::ByteBufferDynamic &buffer, const core::TimeTick &time_tick) {
  while(true) {
    size_t read_size = buffer.ReadSize();
    if(read_size < common::kProtocolHeadSize) {
      break;
    }
    size_t message_size = common::GetProtocolSize(buffer.ReadPointer());
    if(read_size < message_size) {
      break;
    }
    // Deserialize Message.
    common::Message message;
    if(this->packet_.Deserialize<common::Message>(&message,
          GetProtocolData(buffer.ReadPointer()),
          message_size - common::kProtocolHeadSize) == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [ScheduleClient] Deserialize Message failed.",
          __FILE__, __LINE__, __FUNCTION__);
      break;
    }
    // Deserialize MessageSchedule.
    if(message.type_ != common::MessageType::MESSAGE_TYPE_SCHEDULE) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [ScheduleClient] Message type error, is not MESSAGE_TYPE_SCHEDULE.",
          __FILE__, __LINE__, __FUNCTION__);
      break;
    }
    common::MessageSchedule message_schedule;
    if(this->packet_.Deserialize<common::MessageSchedule>(&message_schedule,
          message.message_.data(), message.message_.size()) == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [ScheduleClient] Deserialize MessageSchedule failed.",
          __FILE__, __LINE__, __FUNCTION__);
      break;
    }
    // Handle message.
    this->DispatchMessage(&message_schedule);
    // Shift read pointer.
    buffer.ReadShift(message_size);
  }
}

void ScheduleClient::OnWriteComplete(const core::TCPConnectionPtr &connection) {
  // AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
  //     "%s:%d (%s) connection response complete.", __FILE__, __LINE__, __FUNCTION__);
}

void ScheduleClient::LoginScheduleServer() {
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[ScheduleClient] send MESSAGE_SCHEDULE_TYPE_LOGIN_REQ to server[%s]",
      this->tcp_client_.GetServerAddress().ToString().c_str());

  common::MessageScheduleLoginReq login;
  login.__set_type_(common::ServerType::SERVER_TYPE_SCHEDULE);
  login.__set_id_(ScheduleServerSingleton::GetScheduleServer()->GetServerID());
  ScheduleServerSingleton::GetScheduleServer()->GetLocalAddress().GetAddressInfo(login.host_, *(core::uint16 *)&login.port_);
  this->Send<common::MessageScheduleLoginReq>(&login, common::MessageScheduleType::MESSAGE_SCHEDULE_TYPE_LOGIN_REQ);
}

void ScheduleClient::LogoutScheduleServer() {
  if(this->login_schedule_ == true) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
        "[ScheduleClient] send MESSAGE_SCHEDULE_TYPE_LOGOUT_REQ to server[%s]",
        this->tcp_client_.GetServerAddress().ToString().c_str());

    common::MessageSchedule message;
    message.__set_type_(common::MessageScheduleType::MESSAGE_SCHEDULE_TYPE_LOGOUT_REQ);
    this->Send(&message);
  }
}

void ScheduleClient::DispatchMessage(const MessageSchedule *message) {
  switch(message->type_) {
    case common::MessageScheduleType::MESSAGE_SCHEDULE_TYPE_LOGIN_RES:
      this->OnLoginScheduleResponse(message->message_.data(), message->message_.size());
      break;

    case common::MessageScheduleType::MESSAGE_SCHEDULE_TYPE_LOGOUT_RES:
      this->OnLogoutScheduleResponse(message->message_.data(), message->message_.size());
      break;

    case common::MessageScheduleType::MESSAGE_SCHEDULE_TYPE_LOGIN_SYNC:
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
          "[ScheduleClient] Receive message MESSAGE_SCHEDULE_TYPE_LOGIN_SYNC, ignore it.");
      break;

    case common::MessageScheduleType::MESSAGE_SCHEDULE_TYPE_LOGOUT_SYNC:
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
          "[ScheduleClient] Receive message MESSAGE_SCHEDULE_TYPE_LOGOUT_SYNC, ignore it.");
      break;

    default:
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [ScheduleClient] receive message type is invalid.", __FILE__, __LINE__, __FUNCTION__);
      break;
  }
}

void ScheduleClient::OnLoginScheduleResponse(const char *message, size_t size) {
  common::MessageScheduleLoginRes res;
  // Deserialize.
  if(this->packet_.Deserialize<common::MessageScheduleLoginRes>(&res, message, size) == true) {
    if(res.result_ == true) {
      this->login_schedule_ = true;
      // Copy schedule's logined server.
      std::vector< ::squirrel::common::ServerSignature>::iterator iterator =
        res.server_signatures_.begin();
      for(; iterator != res.server_signatures_.end(); ++iterator) {
        ScheduleTerminalSetSingleton::GetScheduleTerminalSet().InsertLoginedTerminal(
            iterator->type_, iterator->id_, iterator->host_, iterator->port_);
      }
    } else {
      this->login_schedule_ = false;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR, 
          "%s:%d (%s) [ScheduleClient] login schedule repsonsed, the result is failed.",
          __FILE__, __LINE__, __FUNCTION__);
    }
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR, 
        "%s:%d (%s) [ScheduleClient] Deserialize MessageScheduleLoginRes failed.",
        __FILE__, __LINE__, __FUNCTION__);
  }
}

void ScheduleClient::OnLogoutScheduleResponse(const char *message, size_t size) {
  common::MessageScheduleLogoutRes res;
  // Deserialize.
  if(this->packet_.Deserialize<common::MessageScheduleLogoutRes>(&res, message, size) == true) {
    if(res.result_ == true) {
      this->login_schedule_ = false;
    } else {
      this->login_schedule_ = true;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR, 
          "%s:%d (%s) [ScheduleClient] logout schedule repsonsed, the result is failed.",
          __FILE__, __LINE__, __FUNCTION__);
    }
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR, 
        "%s:%d (%s) [ScheduleClient] Deserialize MessageScheduleLogoutRes failed.",
        __FILE__, __LINE__, __FUNCTION__);
  }
}

}  // namespace server

}  // namespace schedule

}  // namespace squirrel

