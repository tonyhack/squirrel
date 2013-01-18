//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-08-27 21:38:19.
// File name: schedule_circle.h
//
// Description: 
// Define class ScheduleCircle.
//

#ifndef _squirrel_schedule_server__SCHEDULE__CIRCLE__H
#define _squirrel_schedule_server__SCHEDULE__CIRCLE__H

#include <boost/shared_ptr.hpp>

#include "core/event_loop.h"
#include "core/timer_handler.h"
#include "core/base/noncopyable.h"
#include "core/base/types.h"
#include "global/thrift_packet.h"
#include "schedule_server/server/schedule_client.h"
#include "schedule_server/server/server_configure.h"

namespace squirrel {

namespace schedule {

namespace server {

class ScheduleCircle : public core::Noncopyable, public core::TimerHandler {
  enum { TIMER_CONNECTION_TIMEOUT = 0, };
  enum { TIMER_CONNECTION_MSEC = 5000, };
  typedef std::map<core::uint32, ScheduleClientPtr> ScheduleCircleMap;
  typedef std::vector<common::MessageSchedule> MessageScheduleCache;
 public:
  ScheduleCircle();
  virtual ~ScheduleCircle();

  virtual void OnTimer(core::uint32 id);

  bool Initialize(core::EventLoop *loop, const ServerConfigure &configure);

  void Start();
  void Stop();
/*
  // This is triggered by schedule server receiving message.
  void LoginSchedule(const common::MessageScheduleLoginSync *sync);
  void LogoutSchedule(const common::MessageScheduleLogoutSync *sync);
*/
  void OnTerminalLogin(const common::MessageScheduleLoginSync *message);
  void OnTerminalLogout(const common::MessageScheduleLogoutSync *message);
  void OnTransferLoginSchedule(common::MessageScheduleLoginSyncTransfer *message);
  void OnTransferLogoutSchedule(common::MessageScheduleLogoutSyncTransfer *message);

  inline bool CheckLoginState() { return this->current_schedule_->CheckLoginState(); }

 private:
  inline void SetCurrentSchedule(core::uint32 id, const ScheduleClientPtr &schedule) {
    this->current_schedule_id_ = id;
    this->current_schedule_ = schedule;
  }

  void InsertSchedule(const common::MessageScheduleLoginSync *message);
  void RemoveSchedule(const common::MessageScheduleLogoutSync *message);

  // Calculate the next node.
  void ResetNextNode();

  void OnConnection(const core::TCPConnectionPtr &connection);

  void OnConnectTimeout();
  void OnDisconnected();

  // Cache message.
  template <typename T>
  void CacheMessage(const T *message, common::MessageScheduleType::type type) {
    size_t size = 0;
    const char *msg_ptr = this->packet_.Serialize<T>(message, size);
    if(msg_ptr) {
      common::MessageSchedule message_schedule;
      message_schedule.__set_type_(type);
      message_schedule.__isset.message_ = true;
      message_schedule.message_.assign(msg_ptr, size);
      this->message_cache_.push_back(message_schedule);
    } else {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) Serialize MessageSchedule[type=%d] failed.",
          __FILE__, __LINE__, __FUNCTION__, type);
    }
  }

  // Send message.
  template <typename T>
  void Send(const T *message, common::MessageScheduleType::type type) {
    // If connected, sending immediately, otherwise, sending to cache.
    if(this->current_schedule_->CheckLoginState()) {
      this->current_schedule_->Send<T>(message, type);
    } else {
      // cache message.
      this->CacheMessage<T>(message, type);
    }
  }

  // Absorb message in cache.
  void AbsorbCache();

  core::EventLoop *loop_;

  // Schedule id in configure file.
  core::uint32 configure_schedule_;
  // This default will be change by OnScheduleClientLogin.
  core::uint32 default_schedule_;

  core::uint32 current_schedule_id_;
  ScheduleClientPtr current_schedule_;
  ScheduleCircleMap circles_;

  // Message cache.
  MessageScheduleCache message_cache_;

  // Thrift tool/
  global::ThriftPacket packet_;
};

}  // namespace server

}  // namespace schedule

}  // namespace squirrel

#endif  // _squirrel_schedule_server__SCHEDULE__CIRCLE__H

