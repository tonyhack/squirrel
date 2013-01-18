//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-08-31 15:21:51.
// File name: schedule_client.cc
//
// Description: 
// Define class ScheduleClient.
//

#include "schedule_server/client/schedule_client.h"

#include <boost/bind.hpp>

#include "core/tcp_client.h"
#include "core/tcp_connection.h"
#include "core/base/noncopyable.h"
#include "common/message_types.h"
#include "common/protocol.h"
#include "global/thrift_packet.h"

namespace squirrel {

namespace schedule {

namespace client {

ScheduleClient *ScheduleClientSingleton::client_ = NULL;

ScheduleClient::ScheduleClient() : loop_(NULL),
  host_type_(common::ServerType::SERVER_TYPE_MAX), host_id_(0),
  default_session_id_(0), current_session_id_(0) {}

ScheduleClient::~ScheduleClient() {}

void ScheduleClient::OnTimer(core::uint32 id) {
  if(id == TIMER_CONNECTION_TIMEOUT) {
    this->OnConnectTimeout();
  }
}

bool ScheduleClient::Initialize(core::EventLoop *loop, core::uint32 app_server_id,
    common::ServerType::type app_server_type, const core::InetAddress &local_address,
    core::uint32 schedule_id, const core::InetAddress &schedule_address) {
  assert(loop);
  this->loop_ = loop;
  this->host_type_ = app_server_type;
  this->host_id_ = app_server_id;
  local_address.GetAddressInfo(this->host_ip_, this->host_port_);
  ScheduleClientSession *session = new (std::nothrow) ScheduleClientSession(
      schedule_address, "ScheduleClientSession");
  if(session) {
    if(session->Initialize(this->loop_)) {
      this->SetCurrentSession(schedule_id, ScheduleClientSessionPtr(session));
      this->default_session_id_ = schedule_id;
      this->sessions_.insert(std::make_pair(schedule_id, this->current_session_));
      return true;
    } else {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [ScheduleClient] Initialize ScheduleClientSession failed.",
          __FILE__, __LINE__, __FUNCTION__);
      delete session;
      return false;
    }
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [ScheduleClient] Allocate ScheduleClientSession failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
}

void ScheduleClient::Start() {
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[ScheduleClient] Start and connect to address [%s].",
      this->current_session_->GetServerAddress().ToString().c_str());
  this->current_session_->Start();
}

void ScheduleClient::Stop() {
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[ScheduleClient] ScheduleClient stopped [%s].",
      this->current_session_->GetServerAddress().ToString().c_str());

  this->current_session_->Stop();
}

void ScheduleClient::OnConnection(const core::TCPConnectionPtr &connection) {
  if(connection->CheckConnected()) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
        "[ScheduleClient] ScheduleClient's connection established.");
    this->loop_->RemoveTimer(TIMER_CONNECTION_TIMEOUT, this);
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
        "[ScheduleClient] ScheduleClient's connection disconnected.");
    this->OnDisconnected();
  }
}

void ScheduleClient::OnLoginScheduleSync(const common::MessageScheduleLoginSync *message) {
  if((core::uint32)message->id_ != this->host_id_ || message->type_ != this->host_type_) {
    if(message->type_ == common::ServerType::SERVER_TYPE_SCHEDULE) {
      ClientSessionMap::iterator iterator = this->sessions_.find(message->id_);
      if(iterator == this->sessions_.end()) {
        ScheduleClientSession *session = new (std::nothrow) ScheduleClientSession(
            core::InetAddress(message->host_, message->port_), "ScheduleClientSession");
        if(session == NULL) {
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR, 
              "%s:%d (%s) [ScheduleClient] Allocate ScheduleClientSession failed.",
              __FILE__, __LINE__, __FUNCTION__);
          return ;
        }
        if(session->Initialize(this->loop_) == false) {
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR, 
              "%s:%d (%s) [ScheduleClient] Initialize ScheduleClientSession failed.",
              __FILE__, __LINE__, __FUNCTION__);
          return ;
        }
        this->sessions_.insert(std::make_pair(message->id_, session));
      }
    } else {
      this->join_schedule_callback_(message);
    }
  }
}

void ScheduleClient::OnLogoutScheduleSync(const common::MessageScheduleLogoutSync *message) {
  if((core::uint32)message->id_ != this->host_id_ || message->type_ != this->host_type_) {
    if(message->type_ == common::ServerType::SERVER_TYPE_SCHEDULE) {
      if((core::uint32)message->id_ != this->default_session_id_) {
        this->sessions_.erase(message->id_);
        if((core::uint32)message->id_ == this->current_session_id_) {
          this->ResetNextNode();
        }
      }
    } else {
      this->disjoin_schedule_callback_(message);
    }
  }
}

void ScheduleClient::OnDisconnected() {
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[ScheduleClient] ScheduleClient disconnected address [%s].",
      this->current_session_->GetServerAddress().ToString().c_str());
  this->ResetNextNode();
}

void ScheduleClient::OnConnectTimeout() {
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[ScheduleClient] ScheduleClient connect address [%s] timeout.",
      this->current_session_->GetServerAddress().ToString().c_str());
  this->current_session_->Stop();
  this->ResetNextNode();
}

void ScheduleClient::ResetNextNode() {
  ClientSessionMap::iterator iterator = this->sessions_.find(this->current_session_id_);
  if(iterator != this->sessions_.end()) {
    if(++iterator == this->sessions_.end()) {
      iterator = this->sessions_.begin();
    }
    assert(iterator != this->sessions_.end());
    this->SetCurrentSession(iterator->first, iterator->second);
  }
  this->current_session_->Start();
  this->loop_->AddTimer(TIMER_CONNECTION_TIMEOUT, TIMER_CONNECTION_MSEC,
      1, this, "ScheduleClient::Start");
}

}  // namespace client

}  // namespace schedule

}  // namespace squirrel

