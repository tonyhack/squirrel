//
// Summary: server core source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-08-27 18:47:51.
// File name: schedule_terminal_set.cc
//
// Description: 
// Define class ScheduleTerminalSet.
//

#include "schedule_server/server/schedule_terminal_set.h"

#include "common/message_types.h"
#include "core/base/auto_mutex.h"
#include "schedule_server/server/schedule_server.h"

namespace squirrel {

namespace schedule {

namespace server {

ScheduleTerminalSet::ScheduleTerminalSet() {}
ScheduleTerminalSet::~ScheduleTerminalSet() {}

bool ScheduleTerminalSet::LoginScheduleServer(const ScheduleTerminalPtr &terminal) {
  if(terminal->GetServerType() == common::ServerType::SERVER_TYPE_SCHEDULE ||
      ScheduleServerSingleton::GetScheduleServer()->GetScheduleCircle().CheckLoginState()) {
    ScheduleKey key(terminal->GetServerType(), terminal->GetServerID());
    // core::AutoMutex auto_mutext(&this->mutex_);
    if(this->schedule_terminals_.find(key) == this->schedule_terminals_.end()) {
      // Insert into logined this schedule server's list.
      this->schedule_terminals_.insert(std::make_pair(key, terminal));
      // Insert into logined system terminals list.
      this->InsertLoginedTerminal(terminal->GetServerType(), terminal->GetServerID(),
          terminal->GetServerHost(), terminal->GetServerPort());
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
          "[ScheduleTerminalSet] [host=%s:%d type=%d id=%u] login schedule server succeed.",
          terminal->GetServerHost().c_str(), terminal->GetServerPort(),
          terminal->GetServerType(), terminal->GetServerID());
      return true;
    } else {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [ScheduleTerminalSet] [host=%s:%d type=%d id=%u] login schedule server repeatedly.",
          __FILE__, __LINE__, __FUNCTION__, terminal->GetServerHost().c_str(),
          terminal->GetServerPort(), terminal->GetServerType(), terminal->GetServerID());
      return false;
    }
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
        "[ScheduleTerminalSet] Schedule server has not logined.");
    return false;
  }
}

bool ScheduleTerminalSet::LogoutScheduleServer(const ScheduleTerminalPtr &terminal) {
  ScheduleKey key(terminal->GetServerType(), terminal->GetServerID());
  // core::AutoMutex auto_mutext(&this->mutex_);
  ScheduleTerminalMap::iterator iterator = this->schedule_terminals_.find(key);
  if(iterator != this->schedule_terminals_.end()) {
    // Remove from logined this schedule server's list.
    this->schedule_terminals_.erase(iterator);
    // Remove from logined system terminals list.
    this->RemoveLoginedTerminal(terminal->GetServerType(), terminal->GetServerID());
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
        "[ScheduleTerminalSet] [host=%s:%d type=%d id=%u] logout server succeed.",
        terminal->GetServerHost().c_str(),
        terminal->GetServerPort(), terminal->GetServerType(), terminal->GetServerID());
    return true;
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [ScheduleTerminalSet] [host=%s:%d type=%d id=%u] logout server failed.",
        __FILE__, __LINE__, __FUNCTION__, terminal->GetServerHost().c_str(),
        terminal->GetServerPort(), terminal->GetServerType(), terminal->GetServerID());
    return false;
  }
}

ScheduleTerminalPtr ScheduleTerminalSet::GetScheduleTerminal(ScheduleKey key) {
  // core::AutoMutex auto_mutext(&this->mutex_);
  ScheduleTerminalMap::iterator iterator = this->schedule_terminals_.find(key);
  if(iterator != this->schedule_terminals_.end()) {
    return iterator->second;
  } else {
    return ScheduleTerminalPtr();
  }
}

void ScheduleTerminalSet::BroadcastMessage(const char *message, size_t size) {
  // core::AutoMutex auto_mutext(&this->mutex_);
  ScheduleTerminalMap::const_iterator iterator = this->schedule_terminals_.begin();
  for(; iterator != this->schedule_terminals_.end(); ++iterator) {
    if(iterator->first.first != common::ServerType::SERVER_TYPE_SCHEDULE) {
      iterator->second->Send(message, size);
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
          "[ScheduleTerminalSet] Broadcast message[size=%ld] to [type=%d, id=%d]",
          size, iterator->first.first, iterator->first.second);
    }
  }
}

void ScheduleTerminalSet::BroadcastMessage(const MessageSchedule *message_schedule) {
  size_t size = 0;
  const char *msg_ptr = this->packet_.Serialize<common::MessageSchedule>(message_schedule, size);
  if(msg_ptr) {
    common::Message message;
    message.__set_type_(MessageType::MESSAGE_TYPE_SCHEDULE);
    message.message_.assign(msg_ptr, size);
    msg_ptr = this->packet_.Serialize<common::Message>(&message, size);
    if(msg_ptr) {
      this->BroadcastMessage(msg_ptr, size);
    } else {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [ScheduleTerminalSet] MessageSchedule failed.", __FILE__, __LINE__, __FUNCTION__);
    }
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [ScheduleTerminalSet] Message failed.", __FILE__, __LINE__, __FUNCTION__);
  }
}

void ScheduleTerminalSet::InsertLoginedTerminal(ServerType::type type, core::uint32 id,
    const std::string &host, core::uint16 port) {
  ScheduleKey key = std::make_pair(type, id);
  LoginedTerminalMap::iterator iterator = this->logined_terminals_.find(key);
  if(iterator == this->logined_terminals_.end()) {
    this->logined_terminals_.insert(std::make_pair(key, std::make_pair(host, port)));
  }
#ifdef _SQUIRREL_TEST
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
      "[_SQUIRREL_TEST ScheduleTerminalSet logined_terminals_ insert] begin ---------");
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
      "[_SQUIRREL_TEST ScheduleTerminalSet] insert [type=%d, id=%d, host=%s, port=%d]",
      type, id, host.c_str(), port);
  iterator = this->logined_terminals_.begin();
  for(; iterator != this->logined_terminals_.end(); ++iterator) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
        "[_SQUIRREL_TEST ScheduleTerminalSet] type=%d id=%d host=%s port=%d",
        iterator->first.first, iterator->first.second,
        iterator->second.first.c_str(), iterator->second.second);
  }
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
      "[_SQUIRREL_TEST ScheduleTerminalSet logined_terminals_ insert] end ---------");
#endif  // _SQUIRREL_TEST
}

void ScheduleTerminalSet::RemoveLoginedTerminal(ServerType::type type, core::uint32 id) {
  ScheduleKey key = std::make_pair(type, id);
  this->logined_terminals_.erase(key);
#ifdef _SQUIRREL_TEST
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
      "[_SQUIRREL_TEST ScheduleTerminalSet logined_terminals_ remove] begin ---------");
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
      "[_SQUIRREL_TEST ScheduleTerminalSet] remove [type=%d, id=%d]", type, id);
  LoginedTerminalMap::iterator iterator = this->logined_terminals_.begin();
  for(; iterator != this->logined_terminals_.end(); ++iterator) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
        "[_SQUIRREL_TEST ScheduleTerminalSet] type=%d id=%d host=%s port=%d",
        iterator->first.first, iterator->first.second,
        iterator->second.first.c_str(), iterator->second.second);
  }
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
      "[_SQUIRREL_TEST ScheduleTerminalSet logined_terminals_ remove] end ---------");
#endif  // _SQUIRREL_TEST
}

ScheduleTerminalSet ScheduleTerminalSetSingleton::schedule_terminal_set_;

}  // namespace server

}  // namespace schedule

}  // namespace squirrel

