//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-08-27 21:39:20.
// File name: schedule_terminal.cc
//
// Description: 
// Define class ScheduleTerminal.
//

#include "schedule_server/server/schedule_terminal.h"

#include "schedule_server/server/schedule_server.h"
#include "schedule_server/server/schedule_terminal_set.h"
#include "common/message_types.h"
#include "common/protocol.h"

namespace squirrel {

namespace schedule {

namespace server {

ScheduleTerminal::ScheduleTerminal() : type_(common::ServerType::SERVER_TYPE_MAX), id_(0), login_state_(false) {}
ScheduleTerminal::~ScheduleTerminal() {}

void ScheduleTerminal::Send(const char *message, size_t size) {
  assert(this->connection_);
  common::ProtocolHead head = size;
/*
  this->buffer_.PreWrite(sizeof(common::kProtocolHeadSize) + size);
  memcpy(this->buffer_.HeadPointer(), &head, sizeof(common::ProtocolHead));
  memcpy(this->buffer_.RandomPointer(sizeof(common::ProtocolHead)), message, size);
  this->connection_->SendMessage(this->buffer_.HeadPointer(),
      sizeof(common::kProtocolHeadSize) + size);
*/
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
      "[ScheduleTerminal] Send message head [size=%ld].", sizeof(common::ProtocolHead));
  this->connection_->SendMessage((const char *)&head, sizeof(common::ProtocolHead));
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
      "[ScheduleTerminal] Send message data [size=%ld].", size);
  this->connection_->SendMessage(message, size);
}

void ScheduleTerminal::Send(const MessageSchedule *message_schedule) {
  size_t size = 0;
  const char *msg_ptr = this->packet_.Serialize<common::MessageSchedule>(message_schedule, size);
  if(msg_ptr) {
    common::Message message;
    message.__set_type_(MessageType::MESSAGE_TYPE_SCHEDULE);
    message.message_.assign(msg_ptr, size);
    msg_ptr = this->packet_.Serialize<common::Message>(&message, size);
    if(msg_ptr) {
      this->Send(msg_ptr, size);
    } else {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [ScheduleTerminal] Serialize MessageSchedule failed.", __FILE__, __LINE__, __FUNCTION__);
    }
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [ScheduleTerminal] Serialize Message failed.", __FILE__, __LINE__, __FUNCTION__);
  }
}

void ScheduleTerminal::OnMessage(const MessageSchedule *message) {
  switch(message->type_) {
    case common::MessageScheduleType::MESSAGE_SCHEDULE_TYPE_LOGIN_REQ:
      this->OnTerminalLogin(message->message_.data(), message->message_.size());
      break;

    case common::MessageScheduleType::MESSAGE_SCHEDULE_TYPE_LOGOUT_REQ:
      this->OnTerminalLogout(message->message_.data(), message->message_.size());
      break;

    case common::MessageScheduleType::MESSAGE_SCHEDULE_TYPE_LOGIN_SYNC_TRANSFER:
      this->OnTerminalTransferLoginSync(message->message_.data(), message->message_.size());
      break;

    case common::MessageScheduleType::MESSAGE_SCHEDULE_TYPE_LOGOUT_SYNC_TRANSFER:
      this->OnTerminalTransferLogoutSync(message->message_.data(), message->message_.size());
      break;

    default: 
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [ScheduleTerminal] message type is invalid.", __FILE__, __LINE__, __FUNCTION__);
      break;
  }
}

void ScheduleTerminal::OnDisConnnect() {
  // Logout schedule server.
  if(this->CheckLoginState()) {
    if(ScheduleTerminalSetSingleton::GetScheduleTerminalSet().LogoutScheduleServer(shared_from_this())) {
      this->login_state_ = false;

      // Logout broadcast.
      common::MessageScheduleLogoutSync sync;
      sync.__set_type_(this->GetServerType());
      sync.__set_id_(this->GetServerID());

      // Broadcast.
      ScheduleTerminalSetSingleton::GetScheduleTerminalSet().BroadcastMessage<
        common::MessageScheduleLogoutSync>(&sync,
            common::MessageScheduleType::MESSAGE_SCHEDULE_TYPE_LOGOUT_SYNC);

      // Message will be transfer in the schedule circle.
      ScheduleServerSingleton::GetScheduleServer()->GetScheduleCircle().OnTerminalLogout(&sync);
    } else {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [ScheduleTerminal] LogoutScheduleServer failed.", __FILE__, __LINE__, __FUNCTION__);
    }
  }
}

void ScheduleTerminal::OnTerminalLogin(const char *message, size_t size) {
  common::MessageScheduleLoginReq msg;
  // Deserialize.
  if(this->packet_.Deserialize<common::MessageScheduleLoginReq>(&msg, message, size) == true) {
    // Set properties.
    this->SetServerType(msg.type_);
    this->SetServerID(msg.id_);
    this->SetServerHost(msg.host_);
    this->SetServerPort(msg.port_);

    // Login response.
    common::MessageScheduleLoginRes response;

    // Login schedule server.
    if(ScheduleTerminalSetSingleton::GetScheduleTerminalSet().LoginScheduleServer(shared_from_this())) {
      // login success.
      this->login_state_ = true;
      response.__set_result_(true);

      // Logined terminals.
      ScheduleTerminalSet::LoginedTerminalMap &logined_terminals =
        ScheduleTerminalSetSingleton::GetScheduleTerminalSet().GetLoginedTerminals();
      if(logined_terminals.empty() == false) {
        response.__isset.server_signatures_ = true;
        common::ServerSignature signature;
        ScheduleTerminalSet::LoginedTerminalMap::iterator iterator = logined_terminals.begin();
        for(; iterator != logined_terminals.end(); ++iterator) {
          signature.type_ = iterator->first.first;
          signature.id_ = iterator->first.second;
          if(this->GetServerType() != signature.type_ ||
              this->GetServerID() != (core::uint32)signature.id_) {
            signature.host_ = iterator->second.first;
            signature.port_ = iterator->second.second;
            response.server_signatures_.push_back(signature);
          }
        }
      }

      // Login broadcast.
      common::MessageScheduleLoginSync sync;
      sync.__set_type_(this->GetServerType());
      sync.__set_id_(this->GetServerID());
      sync.__set_host_(this->GetServerHost());
      sync.__set_port_(this->GetServerPort());
      ScheduleTerminalSetSingleton::GetScheduleTerminalSet().BroadcastMessage<
        common::MessageScheduleLoginSync>(&sync,
            common::MessageScheduleType::MESSAGE_SCHEDULE_TYPE_LOGIN_SYNC);

      // Message will be transfer in the schedule circle.
      ScheduleServerSingleton::GetScheduleServer()->GetScheduleCircle().OnTerminalLogin(&sync);
    } else {
      // Login failed.
      response.__set_result_(false);
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [ScheduleTerminal] LoginScheduleServer failed.",
          __FILE__, __LINE__, __FUNCTION__);
    }
    // Response.
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
        "[ScheduleTerminal] Send MESSAGE_SCHEDULE_TYPE_LOGIN_RES to terminal [%s:%d]",
        this->GetServerHost().c_str(), this->GetServerPort());
    this->Send<common::MessageScheduleLoginRes>(&response,
        common::MessageScheduleType::MESSAGE_SCHEDULE_TYPE_LOGIN_RES);
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [ScheduleTerminal] Deserialize MessageScheduleLoginReq failed.",
        __FILE__, __LINE__, __FUNCTION__);
  }
}

void ScheduleTerminal::OnTerminalLogout(const char *message, size_t size) {
  // logout response.
  common::MessageScheduleLogoutRes response;

  // Logout schedule server.
  if(ScheduleTerminalSetSingleton::GetScheduleTerminalSet().LogoutScheduleServer(shared_from_this())) {
    // Logout success.
    this->login_state_ = false;
    response.__set_result_(true);

    // Login broadcast.
    common::MessageScheduleLogoutSync sync;
    sync.__set_type_(this->GetServerType());
    sync.__set_id_(this->GetServerID());
    ScheduleTerminalSetSingleton::GetScheduleTerminalSet().BroadcastMessage<
      common::MessageScheduleLogoutSync>(&sync,
          common::MessageScheduleType::MESSAGE_SCHEDULE_TYPE_LOGOUT_SYNC);

    // Message will be transfer in the schedule circle.
    ScheduleServerSingleton::GetScheduleServer()->GetScheduleCircle().OnTerminalLogout(&sync);
  } else {
    // Logout failed.
    response.__set_result_(false);
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [ScheduleTerminal] LogoutScheduleServer failed.", __FILE__, __LINE__, __FUNCTION__);
  }
  // Response.
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[ScheduleTerminal] Send MESSAGE_SCHEDULE_TYPE_LOGOUT_RES to terminal [%s:%d]",
      this->GetServerHost().c_str(), this->GetServerPort());
  this->Send<common::MessageScheduleLogoutRes>(&response,
      common::MessageScheduleType::MESSAGE_SCHEDULE_TYPE_LOGOUT_RES);
}

void ScheduleTerminal::OnTerminalTransferLoginSync(const char *message, size_t size) {
  common::MessageScheduleLoginSyncTransfer transfer;
  if(this->packet_.Deserialize<common::MessageScheduleLoginSyncTransfer>(&transfer, message, size) == true) {
    ScheduleServerSingleton::GetScheduleServer()->GetScheduleCircle().OnTransferLoginSchedule(&transfer);
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [ScheduleTerminal] Deserialize MessageScheduleLoginSyncTransfer failed.",
        __FILE__, __LINE__, __FUNCTION__);
  }
}

void ScheduleTerminal::OnTerminalTransferLogoutSync(const char *message, size_t size) {
  common::MessageScheduleLogoutSyncTransfer transfer;
  if(this->packet_.Deserialize<common::MessageScheduleLogoutSyncTransfer>(&transfer, message, size) == true) {
    ScheduleServerSingleton::GetScheduleServer()->GetScheduleCircle().OnTransferLogoutSchedule(&transfer);
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [ScheduleTerminal] Deserialize MessageScheduleLogoutSyncTransfer failed.",
        __FILE__, __LINE__, __FUNCTION__);
  }
}

}  // namespace server

}  // namespace schedule

}  // namespace squirrel

