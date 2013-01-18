//
// Summary: server core source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-08-27 18:49:19.
// File name: schedule_server.cc
//
// Description: 
// Define class ScheduleServer.
//

#include "schedule_server/server/schedule_server.h"

#include <boost/bind.hpp>

#include "common/message_types.h"
#include "common/protocol.h"
#include "core/base/logging.h"
#include "global/async_logging_singleton.h"
#include "schedule_server/server/schedule_terminal.h"

namespace squirrel {

using namespace global;

namespace schedule {

namespace server {

namespace {

struct ScheduleTerminalContext {
  ScheduleTerminalContext(ScheduleTerminal *terminal)
    : terminal_(terminal) {}
  ScheduleTerminalPtr terminal_;
};

}  // namespace

ScheduleServer::ScheduleServer(const core::InetAddress &address,
    const std::string &name) : server_id_(0), server_(address, name) {
  ScheduleServerSingleton::SetScheduleServer(this);
}

ScheduleServer::~ScheduleServer() {}

bool ScheduleServer::Initialize(const ServerConfigure &configure, const std::string &log) {
  if(this->StartAsyncLogging(configure, log) == false) {
    CoreLog(ERROR, "%s:%d (%s) [ScheduleServer] StartAsyncLogging failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  this->thread_pool_.reset(new (std::nothrow) core::EventLoopThreadGroup());
  if(this->thread_pool_ == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [ScheduleServer] Allocate thread pool error.",
        __FILE__, __LINE__, __FUNCTION__);
    return false; 
  }

  if(this->thread_pool_->Initialize(1, "ScheduleThread", true) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [ScheduleServer] Initialize thread pool error.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  if(this->server_.Initialize(this->thread_pool_->GetNextLoop(),
        this->thread_pool_.get()) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [ScheduleServer] Initialize tcp server error.", __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  if(this->client_circle_.Initialize(this->thread_pool_->GetNextLoop(), configure) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [ScheduleServer] Initialize tcp schedule client circle error.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  this->server_id_ = configure.id_;

  this->server_.SetConnectionCallback(boost::bind(
        &ScheduleServer::OnConnection, this, _1));
  this->server_.SetMessageCallback(boost::bind(
        &ScheduleServer::OnMessage, this, _1, _2, _3));
  this->server_.SetWriteCallback(boost::bind(
        &ScheduleServer::OnWriteComplete, this, _1));

  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[ScheduleServer] Schedule server initialize complete.");

  return true;
}

void ScheduleServer::Start() {
  this->thread_pool_->Start();
  this->server_.Start();
  this->client_circle_.Start();
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[ScheduleServer] Schedule server start complete.");
}

void ScheduleServer::Stop() {
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[ScheduleServer] Schedule server stoped.");
  this->client_circle_.Stop();
  this->server_.Stop();
  this->thread_pool_->Stop();
  AsyncLoggingSingleton::GetAsyncLogging()->Stop();
}

bool ScheduleServer::StartAsyncLogging(const ServerConfigure &configure, const std::string &log) {
  AsyncLogging *logging = AsyncLoggingSingleton::GetAsyncLogging();
  if(logging && logging->Initialize(configure.log_ + log, kMaxLoggingSize)) {
    CoreLog(INFO, "%s:%d (%s) [ScheduleServer] Start server log [%s].",
        __FILE__, __LINE__, __FUNCTION__, (configure.log_ + log).c_str());
    logging->Start();
    return true;
  }

  return false;
}

void ScheduleServer::OnConnection(const core::TCPConnectionPtr &connection) {
  if(connection->CheckConnected()) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
        "[ScheduleServer] Connection established.");
    ScheduleTerminal *schedule_terminal = new (std::nothrow) ScheduleTerminal();
    if(schedule_terminal) {
      schedule_terminal->Initialize(connection);
      ScheduleTerminalContext context(schedule_terminal);
      connection->GetContext() = context;
    } else {
      connection->Shutdown();
    }
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
        "[ScheduleServer] Connection disconnected.");
    ScheduleTerminalContext *context =
      &boost::any_cast<ScheduleTerminalContext &>(connection->GetContext());
    if(context) {
      context->terminal_->OnDisConnnect();
      connection->ClearContext();
    }
  }
}

void ScheduleServer::OnMessage(const core::TCPConnectionPtr &connection,
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
          GetProtocolData(buffer.ReadPointer()),
          message_size - common::kProtocolHeadSize) == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [ScheduleServer] Deserialize Message failed.",
          __FILE__, __LINE__, __FUNCTION__);
      break;
    }
    // Deserialize MessageSchedule.
    if(message.type_ != common::MessageType::MESSAGE_TYPE_SCHEDULE) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [ScheduleServer] Message type error, is not MESSAGE_TYPE_SCHEDULE.",
          __FILE__, __LINE__, __FUNCTION__);
      break;
    }
    common::MessageSchedule message_schedule;
    if(this->packet_.Deserialize<common::MessageSchedule>(&message_schedule,
          message.message_.data(), message.message_.size()) == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [ScheduleServer] Deserialize MessageSchedule failed.",
          __FILE__, __LINE__, __FUNCTION__);
      break;
    }
    // Get ScheduleTerminal.
    assert(connection);
    if(connection->GetContext().empty() == true) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [ScheduleServer] Connection's context is empty.",
          __FILE__, __LINE__, __FUNCTION__);
      break;
    }
    ScheduleTerminalContext *context =
      &boost::any_cast<ScheduleTerminalContext &>(connection->GetContext());
    assert(context->terminal_);
    // OnMessage.
    context->terminal_->OnMessage(&message_schedule);
    // Shift read pointer.
    buffer.ReadShift(message_size);
  }
}

void ScheduleServer::OnWriteComplete(const core::TCPConnectionPtr &connection){
  // AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
  //     "%s:%d (%s) connection response complete.", __FILE__, __LINE__, __FUNCTION__);
}

ScheduleServer *ScheduleServerSingleton::server_ = NULL;

}  // namespace server

}  // namespace schedule

}  // namespace squirrel

