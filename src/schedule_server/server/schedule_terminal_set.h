//
// Summary: server core source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-08-27 18:47:40.
// File name: schedule_terminal_set.h
//
// Description: 
// Define class ScheduleTerminalSet.
//

#ifndef _squirrel_schedule_server__SCHEDULE__TERMINAL__SET__H
#define _squirrel_schedule_server__SCHEDULE__TERMINAL__SET__H

#include "common/message_schedule_types.h"
#include "common/server_type_types.h"
#include "core/base/mutex.h"
#include "core/base/noncopyable.h"
#include "global/async_logging_singleton.h"
#include "global/thrift_packet.h"
#include "schedule_server/server/schedule_terminal.h"

namespace squirrel {

using namespace common;
using namespace global;

namespace schedule {

namespace server {

class ScheduleTerminalSet : public core::Noncopyable {
  typedef std::pair<ServerType::type, core::uint32> ScheduleKey;
  typedef std::map<ScheduleKey, ScheduleTerminalPtr> ScheduleTerminalMap;
 public:
  typedef std::pair<std::string, core::uint16> ScheduleInformation;
  typedef std::map<ScheduleKey, ScheduleInformation> LoginedTerminalMap;
  ScheduleTerminalSet();
  ~ScheduleTerminalSet();

  bool LoginScheduleServer(const ScheduleTerminalPtr &terminal);
  bool LogoutScheduleServer(const ScheduleTerminalPtr &terminal);

  ScheduleTerminalPtr GetScheduleTerminal(ScheduleKey key);

  void BroadcastMessage(const char *message, size_t size);
  void BroadcastMessage(const MessageSchedule *message);

  template <typename T>
  void BroadcastMessage(const T *message, common::MessageScheduleType::type type) {
    size_t size = 0;
    const char *msg_ptr = this->packet_.Serialize<T>(message, size);
    if(msg_ptr) {
      common::MessageSchedule message_schedule;
      message_schedule.__set_type_(type);
      message_schedule.__isset.message_ = true;
      message_schedule.message_.assign(msg_ptr, size);
      this->BroadcastMessage(&message_schedule);
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
          "[ScheduleTerminalSet] Broadcast message type [%d] over.", type);
    } else {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) Serialize MessageScheduleXXX[type=%d] failed.",
          __FILE__, __LINE__, __FUNCTION__, type);
    }
  }

  void InsertLoginedTerminal(ServerType::type type, core::uint32 id,
      const std::string &host, core::uint16 port);
  void RemoveLoginedTerminal(ServerType::type type, core::uint32 id);

  LoginedTerminalMap &GetLoginedTerminals() { return this->logined_terminals_; }

 private:
  // Terminals login this schedule server.
  ScheduleTerminalMap schedule_terminals_;
  global::ThriftPacket packet_;
  core::Mutex mutex_;

  // All the terminals login any schedule server.
  LoginedTerminalMap logined_terminals_;
};

class ScheduleTerminalSetSingleton : public core::Noncopyable {
 public:
  ScheduleTerminalSetSingleton() {}
  ~ScheduleTerminalSetSingleton() {}

  static inline ScheduleTerminalSet &GetScheduleTerminalSet() {
    return schedule_terminal_set_;
  }

 private:
  static ScheduleTerminalSet schedule_terminal_set_;
};

}  // namespace server

}  // namespace schedule

}  // namespace squirrel

#endif  // _squirrel_schedule_server__SCHEDULE__TERMINAL__SET__H

