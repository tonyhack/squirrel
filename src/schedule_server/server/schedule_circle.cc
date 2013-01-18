//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-08-27 22:00:42.
// File name: schedule_circle.cc
//
// Description: 
// Define class ScheduleCircle.
//

#include "schedule_server/server/schedule_circle.h"

#include <boost/bind.hpp>

#include "core/inet_address.h"
#include "global/async_logging_singleton.h"
#include "schedule_server/server/schedule_server.h"

namespace squirrel {

using namespace global;

namespace schedule {

namespace server {

ScheduleCircle::ScheduleCircle() : loop_(NULL),
  configure_schedule_(0), default_schedule_(0) {}

ScheduleCircle::~ScheduleCircle() {}

bool ScheduleCircle::Initialize(core::EventLoop *loop, const ServerConfigure &configure) {
  assert(loop);
  this->loop_ = loop;
  ScheduleClient *configure_schedule = new (std::nothrow) ScheduleClient(
      core::InetAddress(configure.default_schedule_ip_, configure.default_schedule_port_),
      "ScheduleClient");
  if(configure_schedule) {
    if(configure_schedule->Initialize(this->loop_)) {
      this->configure_schedule_ = configure.default_schedule_id_;
      this->default_schedule_ = configure.default_schedule_id_;
      this->SetCurrentSchedule(this->configure_schedule_, ScheduleClientPtr(configure_schedule));
      this->circles_.insert(std::make_pair(this->configure_schedule_, this->current_schedule_));
      return true;
    } else {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [ScheduleCircle] Initialize ScheduleClient failed.",
          __FILE__, __LINE__, __FUNCTION__);
      delete configure_schedule;
      return false;
    }
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [ScheduleCircle] Allocate ScheduleClient failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
}

void ScheduleCircle::Start() {
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[ScheduleCircle] ScheduleCircle begin to connect to address [%s].",
      this->current_schedule_->GetServerAddress().ToString().c_str());

  this->loop_->AddTimer(TIMER_CONNECTION_TIMEOUT, TIMER_CONNECTION_MSEC,
      1, this, "ScheduleCircle::Start");
  // Set callback.
  this->current_schedule_->SetConnectionCallback(
      boost::bind(&ScheduleCircle::OnConnection, this, _1));
  // Start to connect.
  this->current_schedule_->Start();
}

void ScheduleCircle::Stop() {
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[ScheduleCircle] ScheduleClient stopped [%s].",
      this->current_schedule_->GetServerAddress().ToString().c_str());

  this->current_schedule_->ResetConnectionCallback();
  this->current_schedule_->Stop();
  // TODO: clear the logined_terminals_ in class "ScheduleTerminalSet",
  // and kick out all the client connecting to this schedule server.
}

void ScheduleCircle::OnTimer(core::uint32 id) {
  if(id == TIMER_CONNECTION_TIMEOUT) {
    this->OnConnectTimeout();
  }
}

void ScheduleCircle::OnConnection(const core::TCPConnectionPtr &connection) {
  if(connection->CheckConnected()) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
        "[ScheduleCircle] ScheduleCircle's connection established.");
    this->loop_->RemoveTimer(TIMER_CONNECTION_TIMEOUT, this);
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
        "[ScheduleCircle] ScheduleCircle's connection disconnected.");
    this->OnDisconnected();
  }
}

void ScheduleCircle::OnConnectTimeout() {
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[ScheduleCircle] ScheduleCircle connect address [%s] timeout.",
      this->current_schedule_->GetServerAddress().ToString().c_str());

  // First, stop this connect.
  this->Stop();
  // Second, select a new client to connect,
  // the new client should be the next one in the circles.
  ScheduleCircleMap::iterator iterator = this->circles_.find(this->current_schedule_id_);
  if(iterator != this->circles_.end()) {
    ++iterator;
    if(iterator == this->circles_.end()) {
      iterator = this->circles_.begin();
    }
    assert(iterator != this->circles_.end());
    // Set current schedule and start it.
    this->SetCurrentSchedule(iterator->first, iterator->second);
    this->Start();
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [ScheduleCircle] Can't find current_schedule_id_ in circles_.",
        __FILE__, __LINE__, __FUNCTION__);
  }
}

void ScheduleCircle::OnTerminalLogin(const common::MessageScheduleLoginSync *message) {
  if(this->CheckLoginState() == true) {
    // Insert schedule into circles_.
    if(message->type_ == common::ServerType::SERVER_TYPE_SCHEDULE) {
      this->InsertSchedule(message);
    }

    // OnTerminalLogin, Abosorb message in the cache.
    this->AbsorbCache();

    // Begin to transfer login in schedule circle.
    common::MessageScheduleLoginSyncTransfer transfer;
    transfer.__set_id_(message->id_);
    transfer.__set_type_(message->type_);
    transfer.__set_host_(message->host_);
    transfer.__set_port_(message->port_);
    transfer.__set_transfer_node_count_(1);
    transfer.__set_transfer_schedule_id_(ScheduleServerSingleton::GetScheduleServer()->GetServerID());

    this->Send<common::MessageScheduleLoginSyncTransfer>(&transfer,
        common::MessageScheduleType::MESSAGE_SCHEDULE_TYPE_LOGIN_SYNC_TRANSFER);
  }
}

void ScheduleCircle::OnTerminalLogout(const common::MessageScheduleLogoutSync *message) {
  // Remove schedule from circles_.
  if(message->type_ == common::ServerType::SERVER_TYPE_SCHEDULE) {
    this->RemoveSchedule(message);
  }

  // Begin to transfer login in schedule circle.
  common::MessageScheduleLogoutSyncTransfer transfer;
  transfer.__set_id_(message->id_);
  transfer.__set_type_(message->type_);
  transfer.__set_transfer_node_count_(1);
  transfer.__set_transfer_schedule_id_(ScheduleServerSingleton::GetScheduleServer()->GetServerID());

  this->Send<common::MessageScheduleLogoutSyncTransfer>(&transfer,
      common::MessageScheduleType::MESSAGE_SCHEDULE_TYPE_LOGOUT_SYNC_TRANSFER);
}

void ScheduleCircle::OnTransferLoginSchedule(common::MessageScheduleLoginSyncTransfer *message) {
  core::uint32 server_id = ScheduleServerSingleton::GetScheduleServer()->GetServerID();
  // Check the transfer's finishing.
  if((core::uint32)message->transfer_schedule_id_ != server_id &&
      (core::uint32)message->transfer_node_count_ <= this->circles_.size()) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
        "[ScheduleCircle] transfer schedule's login [type=%d id=%d host=%s port=%d]",
        message->type_, message->id_, message->host_.c_str(), message->port_);
    // Broadcast message to the application servers.
    common::MessageScheduleLoginSync sync;
    sync.__set_id_(message->id_);
    sync.__set_type_(message->type_);
    sync.__set_host_(message->host_);
    sync.__set_port_(message->port_);
    ScheduleTerminalSetSingleton::GetScheduleTerminalSet().BroadcastMessage<common::MessageScheduleLoginSync>(
        &sync, common::MessageScheduleType::MESSAGE_SCHEDULE_TYPE_LOGIN_SYNC);

    // Insert schedule into circles_.
    if(message->type_ == common::ServerType::SERVER_TYPE_SCHEDULE) {
      this->InsertSchedule(&sync);
    }

    // Insert into all logined system's list.
    ScheduleTerminalSetSingleton::GetScheduleTerminalSet().InsertLoginedTerminal(message->type_,
        message->id_, message->host_, message->port_);

    // Continue to transfer.
    message->__set_transfer_node_count_(message->transfer_node_count_ + 1);
    this->Send<common::MessageScheduleLoginSyncTransfer>(message,
        common::MessageScheduleType::MESSAGE_SCHEDULE_TYPE_LOGIN_SYNC_TRANSFER);
  }
}

void ScheduleCircle::OnTransferLogoutSchedule(common::MessageScheduleLogoutSyncTransfer *message) {
  core::uint32 server_id = ScheduleServerSingleton::GetScheduleServer()->GetServerID();
  if((core::uint32)message->transfer_schedule_id_ != server_id &&
      (core::uint32)message->transfer_node_count_ <= this->circles_.size()) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
        "[ScheduleCircle] transfer schedule's logout [id=%d]", message->id_);
    // Broadcast message to the application servers.
    common::MessageScheduleLogoutSync sync;
    sync.__set_id_(message->id_);
    sync.__set_type_(message->type_);
    ScheduleTerminalSetSingleton::GetScheduleTerminalSet().BroadcastMessage<common::MessageScheduleLogoutSync>(
        &sync, common::MessageScheduleType::MESSAGE_SCHEDULE_TYPE_LOGOUT_SYNC);

    // Remove from schedule from circles_.
    if(message->type_ == common::ServerType::SERVER_TYPE_SCHEDULE) {
      this->RemoveSchedule(&sync);
    }

    // Remove from all logined system's list.
    ScheduleTerminalSetSingleton::GetScheduleTerminalSet().RemoveLoginedTerminal(
        message->type_, message->id_);

    if(message->type_ != common::ServerType::SERVER_TYPE_SCHEDULE ||
        (core::uint32)message->id_ != server_id) {
      // Continue to transfer.
      message->__set_transfer_node_count_(message->transfer_node_count_ + 1);
      this->Send<common::MessageScheduleLogoutSyncTransfer>(message,
          common::MessageScheduleType::MESSAGE_SCHEDULE_TYPE_LOGOUT_SYNC_TRANSFER);
    }
  }
}

void ScheduleCircle::InsertSchedule(const common::MessageScheduleLoginSync *message) {
  core::uint32 server_id = ScheduleServerSingleton::GetScheduleServer()->GetServerID();
  if((core::uint32)message->id_ != server_id) {
    ScheduleCircleMap::iterator iterator = this->circles_.find(message->id_);
    if(iterator == this->circles_.end()) {
      ScheduleClient *client = new (std::nothrow) ScheduleClient(
          core::InetAddress(message->host_, message->port_), "ScheduleClient");
      if(client == NULL) {
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR, 
            "%s:%d (%s) [ScheduleCircle] Allocate ScheduleClient failed.",
            __FILE__, __LINE__, __FUNCTION__);
        return ;
      }
      if(client->Initialize(this->loop_) == false) {
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR, 
            "%s:%d (%s) [ScheduleCircle] Initialize ScheduleClient failed.",
            __FILE__, __LINE__, __FUNCTION__);
        delete client;
        return ;
      }
      // Insert new node into circles_.
      this->circles_.insert(std::make_pair(message->id_, client));
    }
    // Research the next node.
    this->ResetNextNode();
  }
}

void ScheduleCircle::RemoveSchedule(const common::MessageScheduleLogoutSync *message) {
  if((core::uint32)message->id_ != this->current_schedule_id_ &&
      (core::uint32)message->id_ != this->configure_schedule_) {
    ScheduleCircleMap::iterator iterator = this->circles_.find(message->id_);
    if(iterator != this->circles_.end()) {
      // Remove the node.
      this->circles_.erase(iterator);
      // Research the next node.
      this->ResetNextNode();
    }
  }
}

void ScheduleCircle::ResetNextNode() {
  core::uint32 server_id = ScheduleServerSingleton::GetScheduleServer()->GetServerID();

  // Find out current server id's next one to connect.
  ScheduleCircleMap::iterator iterator = this->circles_.begin();
  for(; iterator != this->circles_.end(); ++iterator) {
    if(server_id < iterator->first) break;
  }

  // If end(), the next one should be begin(), because it is ring structure.
  if(iterator == this->circles_.end()) {
    iterator = this->circles_.begin();
  }

  // If the current schedule is not the one we just have find out,
  // Stop the current one, and let the new one replace.
  if(this->current_schedule_id_ != iterator->first) {
    this->Stop();
    this->SetCurrentSchedule(iterator->first, iterator->second);
    this->Start();
    // Update default schedule.
    this->default_schedule_ = iterator->first;
  }
}

void ScheduleCircle::OnDisconnected() {
  this->Stop();
  // When disconnected, we should choose the default schedule to connect,
  // default schedule always is the first choice when the circle is complete connected.
  // when default one can not be connected, we will loop to choose the next one until
  // we have a available one.
  ScheduleCircleMap::iterator iterator = this->circles_.find(this->default_schedule_);
  if(iterator != this->circles_.end()) {
    this->SetCurrentSchedule(iterator->first, iterator->second);
    this->Start();
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [ScheduleCircle] Can't find default_schedule_id_ in circles_.",
        __FILE__, __LINE__, __FUNCTION__);
  }
}

void ScheduleCircle::AbsorbCache() {
  if(this->current_schedule_->CheckLoginState()) {
    MessageScheduleCache::iterator iterator = this->message_cache_.begin();
    for(; iterator != this->message_cache_.end(); ++iterator) {
      this->current_schedule_->Send(&(*iterator));
    }
    this->message_cache_.clear();
  }
}

}  // namespace server

}  // namespace schedule

}  // namespace squirrel

