//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-04 10:30:43.
// File name: request_terminal.cc
//
// Description: 
// Define class Terminal.
//

#include "data_proxy_server/server/request_terminal.h"

#include "common/async_request_object_types.h"
#include "common/keyvalue_message_types.h"
#include "common/number_message_types.h"
#include "common/list_message_types.h"
#include "common/map_message_types.h"
#include "common/message_types.h"
#include "common/protocol.h"
#include "data_proxy_server/server/request_service.h"
#include "data_proxy_server/server/request_terminal_set.h"
#include "global/elf_hash.h"

namespace squirrel {

namespace dataproxy {

namespace server {

RequestTerminal::RequestTerminal() : guid_(0),
  type_(common::ServerType::SERVER_TYPE_MAX),
  id_(0), port_(0), login_state_(false) {}

RequestTerminal::~RequestTerminal() {}

void RequestTerminal::OnMessage(const common::MessageDataProxy *message) {
  switch(message->type_) {
    case common::MessageDataProxyType::MESSAGE_DATA_PROXY_TYPE_LOGIN_REQ:
      this->OnTerminalLogin(message->message_.data(), message->message_.size());
      break;

    case common::MessageDataProxyType::MESSAGE_DATA_PROXY_TYPE_LOGOUT_REQ:
      this->OnTerminalLogout(message->message_.data(), message->message_.size());
      break;

    case common::MessageDataProxyType::MESSAGE_DATA_PROXY_TYPE_DATA_REQUEST:
      this->OnTerminalRequest(message->message_.data(), message->message_.size());
      break;

    default:
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [RequestTerminal] message type is invalid.",
          __FILE__, __LINE__, __FUNCTION__);
      break;
  }
}

void RequestTerminal::OnDisConnnect() {
  if(this->CheckLoginState()) {
    if(RequestTerminalSetSingleton::GetRequestTerminalSet().LogoutDataproxyServer(
          shared_from_this())) {
      this->login_state_ = false;
    } else {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [RequestTerminal] LogoutDataproxyServer failed.",
          __FILE__, __LINE__, __FUNCTION__);
    }
  }
}

void RequestTerminal::SendMessage(const char *message, size_t size) {
  assert(this->connection_);
  common::ProtocolHead head = size;
  {
    core::AutoMutex auto_mutex(&this->mutex_);
    this->mt_buffer_.PreWrite(sizeof(common::kProtocolHeadSize) + size);
    memcpy(this->mt_buffer_.HeadPointer(), &head, sizeof(common::ProtocolHead));
    memcpy(this->mt_buffer_.RandomPointer(sizeof(common::ProtocolHead)), message, size);
    this->connection_->SendMessage(this->mt_buffer_.HeadPointer(),
        sizeof(common::kProtocolHeadSize) + size);
  }
}

void RequestTerminal::Send(const char *message, size_t size) {
  assert(this->connection_);
  common::ProtocolHead head = size;
  this->buffer_.PreWrite(sizeof(common::kProtocolHeadSize) + size);
  memcpy(this->buffer_.HeadPointer(), &head, sizeof(common::ProtocolHead));
  memcpy(this->buffer_.RandomPointer(sizeof(common::ProtocolHead)), message, size);
  this->connection_->SendMessage(this->buffer_.HeadPointer(),
      sizeof(common::kProtocolHeadSize) + size);
}

void RequestTerminal::Send(const common::MessageDataProxy *message_dataproxy) {
  size_t size = 0;
  const char *msg_ptr = this->packet_.Serialize<common::MessageDataProxy>(
      message_dataproxy, size);
  if(msg_ptr) {
    common::Message message;
    message.__set_type_(common::MessageType::MESSAGE_TYPE_DATA_PROXY);
    message.message_.assign(msg_ptr, size);
    msg_ptr = this->packet_.Serialize<common::Message>(&message, size);
    if(msg_ptr) {
      this->Send(msg_ptr, size);
    } else {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [RequestTerminal] Serialize MessageDataProxy failed.",
          __FILE__, __LINE__, __FUNCTION__);
    }
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [RequestTerminal] Serialize Message failed.",
        __FILE__, __LINE__, __FUNCTION__);
  }
}

void RequestTerminal::OnTerminalLogin(const char *message, size_t size) {
  common::MessageDataProxyLoginReq msg;
  if(this->packet_.Deserialize<common::MessageDataProxyLoginReq>(
        &msg, message, size) == true) {
    this->SetServerType(msg.type_);
    this->SetServerID(msg.id_);
    this->SetServerHost(msg.host_);
    this->SetServerPort(msg.port_);

    common::MessageDataProxyLoginRes response;
    if(RequestTerminalSetSingleton::GetRequestTerminalSet().LoginDataproxyServer(
          shared_from_this())) {
      this->login_state_ = true;
      response.__set_result_(true);
    } else {
      response.__set_result_(false);
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [RequestTerminal] LoginDataproxyServer failed.",
          __FILE__, __LINE__, __FUNCTION__);
    }
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
        "[RequestTerminal] Send MESSAGE_DATA_PROXY_TYPE_LOGIN_RES to terminal [%s:%d]",
        this->GetServerHost().c_str(), this->GetServerPort());
    this->Send<common::MessageDataProxyLoginRes>(&response,
        common::MessageDataProxyType::MESSAGE_DATA_PROXY_TYPE_LOGIN_RES);
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [RequestTerminal] Deserialize MessageDataProxyLoginReq failed.",
        __FILE__, __LINE__, __FUNCTION__);
  }
}

void RequestTerminal::OnTerminalLogout(const char *message, size_t size) {
  common::MessageDataProxyLogoutRes response;
  if(RequestTerminalSetSingleton::GetRequestTerminalSet().LogoutDataproxyServer(
        shared_from_this())) {
    this->login_state_ = false;
    response.__set_result_(true);
  } else {
    response.__set_result_(false);
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [RequestTerminal] LogoutDataproxyServer failed.",
        __FILE__, __LINE__, __FUNCTION__);
  }
  this->Send<common::MessageDataProxyLogoutRes>(&response,
      common::MessageDataProxyType::MESSAGE_DATA_PROXY_TYPE_LOGOUT_RES);
}

void RequestTerminal::OnTerminalRequest(const char *message, size_t size) {
#ifdef _TIME_CONSUMING_TEST
  global::TimeConsuming time_consume("RequestTerminal::OnTerminalRequest");
  RequestServiceSingleton::GetRequestService()->request_consume_.SetDebugInfo("Request total consume");
  RequestServiceSingleton::GetRequestService()->request_consume_.ResetConsuming();
#endif
  bool failed = false;
  common::MessageDataProxyDataRequest msg;
  if(this->packet_.Deserialize<common::MessageDataProxyDataRequest>(
        &msg, message, size) == true) {
    // Construct asynchronous request object and do request.
    common::AsyncRequestObject object;
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
        "[RequestTerminal] [20120917] request_id_[%ld]", msg.request_id_);
    object.__set_request_id_(msg.request_id_);
    object.__set_terminal_guid_(this->GetServerGuid());
    object.__set_type_(msg.type_);
    object.__set_storage_type_(msg.storage_type_);
    object.__set_key_(msg.key_);
    object.__set_key_hash_value_(global::elf_hash(msg.key_.data(), msg.key_.size()));
    object.__set_request_(msg.request_);
    if(RequestServiceSingleton::GetRequestService()->Request(
          msg.rd_type_, &object) == false) {
      failed = true;
    }
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [RequestTerminal] Deserialize MessageDataProxyDataRequest failed.",
        __FILE__, __LINE__, __FUNCTION__);
    failed = true;
  }
  if(failed == true) {
    // Failed response.
    common::MessageDataProxyDataResponse response;
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
        "[RequestTerminal] [20120917] request failed request_id_[%ld]", msg.request_id_);
    response.__set_request_id_(msg.request_id_);
    response.__set_type_(msg.type_);
    response.__set_key_(msg.key_);
    // Response value.
    if(msg.type_ == common::RequestType::REQUEST_TYPE_KEYVALUE) {
      common::KeyvalueResponse keyvalue;
      keyvalue.__set_result_(common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_UNKNOWN);
      size_t size = 0;
      const char *msg_ptr = this->packet_.Serialize<common::KeyvalueResponse>(
          &keyvalue, size);
      if(msg_ptr) {
        response.response_.assign(msg_ptr, size);
      } else {
        response.__set_type_(common::RequestType::REQUEST_TYPE_MAX);
      }
    } else if(msg.type_ == common::RequestType::REQUEST_TYPE_NUMBER) {
      common::NumberResponse number;
      number.__set_result_(common::NumberResponseType::NUMBER_RESPONSE_TYPE_UNKNOWN);
      size_t size = 0;
      const char *msg_ptr = this->packet_.Serialize<common::NumberResponse>(
          &number, size);
      if(msg_ptr) {
        response.response_.assign(msg_ptr, size);
      } else {
        response.__set_type_(common::RequestType::REQUEST_TYPE_MAX);
      }
    } else if(msg.type_ == common::RequestType::REQUEST_TYPE_LIST) {
      common::ListResponse list;
      list.__set_result_(common::ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN);
      size_t size = 0;
      const char *msg_ptr = this->packet_.Serialize<common::ListResponse>(
          &list, size);
      if(msg_ptr) {
        response.response_.assign(msg_ptr, size);
      } else {
        response.__set_type_(common::RequestType::REQUEST_TYPE_MAX);
      }
    } else if(msg.type_ == common::RequestType::REQUEST_TYPE_MAP) {
      common::MapResponse map;
      map.__set_result_(common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN);
      size_t size = 0;
      const char *msg_ptr = this->packet_.Serialize<common::MapResponse>(
          &map, size);
      if(msg_ptr) {
        response.response_.assign(msg_ptr, size);
      } else {
        response.__set_type_(common::RequestType::REQUEST_TYPE_MAX);
      }
    } else {
      response.__set_type_(common::RequestType::REQUEST_TYPE_MAX);
    }
    // Send response.
    this->Send<common::MessageDataProxyDataResponse>(&response,
        common::MessageDataProxyType::MESSAGE_DATA_PROXY_TYPE_DATA_RESPONSE);
  }
}

}  // namespace server

}  // namespace dataproxy

}  // namespace squirrel

