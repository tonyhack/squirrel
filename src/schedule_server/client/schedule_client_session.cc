//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-08-31 16:01:15.
// File name: schedule_client_session.cc
//
// Description: 
// Define class ScheduleClientSession.
//

#include "schedule_server/client/schedule_client_session.h"

#include <vector>
#include <boost/bind.hpp>

#include "common/message_types.h"
#include "common/protocol.h"
#include "schedule_server/client/schedule_client.h"

namespace squirrel {

namespace schedule {

namespace client {

ScheduleClientSession::ScheduleClientSession(const core::InetAddress &server_address,
  const std::string &name) : tcp_client_(server_address, name),
  loop_(NULL), login_schedule_(false) {}

ScheduleClientSession::~ScheduleClientSession() {}

bool ScheduleClientSession::Initialize(core::EventLoop *loop) {
  assert(loop);
  if(this->tcp_client_.Initialize(loop) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [ScheduleClientSession] TCPClient initialize failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  this->tcp_client_.SetConnectionCallback(boost::bind(
        &ScheduleClientSession::OnConnection, this, _1));
  this->tcp_client_.SetMessageCallback(boost::bind(
        &ScheduleClientSession::OnMessage, this, _1, _2, _3));
  this->tcp_client_.SetWriteCallback(boost::bind(
        &ScheduleClientSession::OnWriteComplete, this, _1));

  return true;
}

void ScheduleClientSession::Start() {
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[ScheduleClientSession] start to connect [%s].",
      this->tcp_client_.GetServerAddress().ToString().c_str());
  this->tcp_client_.Start();
}
void ScheduleClientSession::Stop() {
  this->login_schedule_ = false;
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[ScheduleClientSession] stopped connection [%s].",
      this->tcp_client_.GetServerAddress().ToString().c_str());
  this->tcp_client_.Stop();
  this->connection_.reset();
}

void ScheduleClientSession::Send(const char *message, size_t size) {
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

void ScheduleClientSession::Send(const common::MessageSchedule *message_schedule) {
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
          "%s:%d (%s) [ScheduleClientSession] Serialize MessageSchedule failed.",
          __FILE__, __LINE__, __FUNCTION__);
    }
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [ScheduleClientSession] Serialize Message failed.",
        __FILE__, __LINE__, __FUNCTION__);
  }
}

void ScheduleClientSession::OnConnection(const core::TCPConnectionPtr &connection) {
  if(connection->CheckConnected()) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
        "[ScheduleClientSession] connection [%s] established.",
        connection->GetPeerAddress().ToString().c_str());
    this->connection_ = connection;
    ScheduleClientSingleton::GetScheduleClient()->OnConnection(connection);
    this->LoginScheduleServer();
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
        "[ScheduleClientSession] connection [%s] disconnected.",
        connection->GetPeerAddress().ToString().c_str());
    this->login_schedule_ = false;
    ScheduleClientSingleton::GetScheduleClient()->OnConnection(connection);
    this->connection_.reset();
  }
}

void ScheduleClientSession::LoginScheduleServer() {
  // Send login request to schedule server.
  common::MessageScheduleLoginReq message;
  message.__set_type_(ScheduleClientSingleton::GetScheduleClient()->GetHostType());
  message.__set_id_(ScheduleClientSingleton::GetScheduleClient()->GetHostID());
  message.__set_host_(ScheduleClientSingleton::GetScheduleClient()->GetHostIP());
  message.__set_port_(ScheduleClientSingleton::GetScheduleClient()->GetHostPort());
  this->Send<common::MessageScheduleLoginReq>(&message,
      common::MessageScheduleType::MESSAGE_SCHEDULE_TYPE_LOGIN_REQ);
  // Log.
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[ScheduleClientSession] Send MESSAGE_SCHEDULE_TYPE_LOGIN_REQ to [%s].",
      this->tcp_client_.GetServerAddress().ToString().c_str());
}

void ScheduleClientSession::OnMessage(const core::TCPConnectionPtr &connection,
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
          "%s:%d (%s) [ScheduleClient] Deserialize Message failed.",
          __FILE__, __LINE__, __FUNCTION__);
      break ;
    }
    // Deserialize MessageSchedule.
    if(message.type_ != common::MessageType::MESSAGE_TYPE_SCHEDULE) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [ScheduleClient] Message type error, is not MESSAGE_TYPE_SCHEDULE.",
          __FILE__, __LINE__, __FUNCTION__);
      break ;
    }
    common::MessageSchedule message_schedule;
    if(this->packet_.Deserialize<common::MessageSchedule>(&message_schedule,
          message.message_.data(), message.message_.size()) == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [ScheduleClient] Deserialize MessageSchedule failed.",
          __FILE__, __LINE__, __FUNCTION__);
      break ;
    }
    // Handle message.
    this->DispatchMessage(&message_schedule);
    // Shift read pointer.
    buffer.ReadShift(message_size);
  }
}

void ScheduleClientSession::OnWriteComplete(const core::TCPConnectionPtr &connection) {}

void ScheduleClientSession::DispatchMessage(const common::MessageSchedule *message) {
  switch(message->type_) {
    case common::MessageScheduleType::MESSAGE_SCHEDULE_TYPE_LOGIN_RES:
      this->OnLoginScheduleResponse(message->message_.data(), message->message_.size());
      break;

    case common::MessageScheduleType::MESSAGE_SCHEDULE_TYPE_LOGOUT_RES:
      this->OnLogoutScheduleResponse(message->message_.data(), message->message_.size());
      break;

    case common::MessageScheduleType::MESSAGE_SCHEDULE_TYPE_LOGIN_SYNC:
      this->OnLoginScheduleSync(message->message_.data(), message->message_.size());
      break;

    case common::MessageScheduleType::MESSAGE_SCHEDULE_TYPE_LOGOUT_SYNC:
      this->OnLogoutScheduleSync(message->message_.data(), message->message_.size());
      break;

    default:
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [ScheduleClient] receive message type is invalid.", __FILE__, __LINE__, __FUNCTION__);
      break;
  }
}

void ScheduleClientSession::OnLoginScheduleResponse(const char *message, size_t size) {
  common::MessageScheduleLoginRes res;
  // Deserialize.
  if(this->packet_.Deserialize<common::MessageScheduleLoginRes>(&res, message, size) == true) {
    if(res.result_ == true) {
      this->login_schedule_ = true;
      // Copy schedule's logined server.
      std::vector< ::squirrel::common::ServerSignature>::iterator iterator =
        res.server_signatures_.begin();
      common::MessageScheduleLoginSync sync;
      for(; iterator != res.server_signatures_.end(); ++iterator) {
        sync.type_ = iterator->type_;
        sync.id_ = iterator->id_;
        sync.host_ = iterator->host_;
        sync.port_ = iterator->port_;
        ScheduleClientSingleton::GetScheduleClient()->OnLoginScheduleSync(&sync);
      }
    } else {
      this->login_schedule_ = false;
      this->Stop();
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR, 
          "%s:%d (%s) [ScheduleClientSession] login schedule repsonsed, the result is failed.",
          __FILE__, __LINE__, __FUNCTION__);
    }
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR, 
        "%s:%d (%s) [ScheduleClientSession] Deserialize MessageScheduleLoginRes failed.",
        __FILE__, __LINE__, __FUNCTION__);
  }
}

void ScheduleClientSession::OnLogoutScheduleResponse(const char *message, size_t size) {
  common::MessageScheduleLogoutRes res;
  // Deserialize.
  if(this->packet_.Deserialize<common::MessageScheduleLogoutRes>(&res, message, size) == true) {
    if(res.result_ == true) {
      this->login_schedule_ = false;
    } else {
      this->login_schedule_ = true;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR, 
          "%s:%d (%s) [ScheduleClientSession] logout schedule repsonsed, the result is failed.",
          __FILE__, __LINE__, __FUNCTION__);
    }
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR, 
        "%s:%d (%s) [ScheduleClientSession] Deserialize MessageScheduleLogoutRes failed.",
        __FILE__, __LINE__, __FUNCTION__);
  }
}

void ScheduleClientSession::OnLoginScheduleSync(const char *message, size_t size) {
  common::MessageScheduleLoginSync sync;
  if(this->packet_.Deserialize<common::MessageScheduleLoginSync>(&sync, message, size) == true) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO, 
        "[ScheduleClientSession] Receive MESSAGE_SCHEDULE_TYPE_LOGIN_SYNC [type=%d, id=%d].",
        sync.type_, sync.id_);
    ScheduleClientSingleton::GetScheduleClient()->OnLoginScheduleSync(&sync);
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [ScheduleClientSession] Deserialize MessageScheduleLoginSync failed.",
        __FILE__, __LINE__, __FUNCTION__);
  }
}

void ScheduleClientSession::OnLogoutScheduleSync(const char *message, size_t size) {
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO, 
      "[ScheduleClientSession] Receive MESSAGE_SCHEDULE_TYPE_LOGOUT_SYNC.");
  common::MessageScheduleLogoutSync sync;
  if(this->packet_.Deserialize<common::MessageScheduleLogoutSync>(&sync, message, size) == true) {
    ScheduleClientSingleton::GetScheduleClient()->OnLogoutScheduleSync(&sync);
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [ScheduleClientSession] Deserialize MessageScheduleLogoutSync failed.",
        __FILE__, __LINE__, __FUNCTION__);
  }
}

}  // namespace client

}  // namespace schedule

}  // namespace squirrel


