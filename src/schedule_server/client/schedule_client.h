//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-08-31 15:21:36.
// File name: schedule_client.h
//
// Description: 
// Define class ScheduleClient.
//

#ifndef _squirrel_schedule_client__SCHEDULE__CLIENT__H
#define _squirrel_schedule_client__SCHEDULE__CLIENT__H

#include <boost/shared_ptr.hpp>

#include "common/message_schedule_types.h"
#include "core/callbacks.h"
#include "core/event_loop.h"
#include "core/inet_address.h"
#include "core/tcp_client.h"
#include "core/tcp_connection.h"
#include "core/timer_handler.h"
#include "core/base/byte_buffer_dynamic.h"
#include "core/base/noncopyable.h"
#include "global/async_logging_singleton.h"
#include "global/thrift_packet.h"
#include "schedule_server/client/schedule_client_session.h"

namespace squirrel {

using namespace global;

namespace schedule {

namespace client {

class ScheduleClient : public core::Noncopyable, public core::TimerHandler {
  enum { TIMER_CONNECTION_TIMEOUT = 0, };
  enum { TIMER_CONNECTION_MSEC = 5000, };
  friend class ScheduleClientSession;
  typedef std::map<core::uint32, ScheduleClientSessionPtr> ClientSessionMap;
  typedef boost::function<void (const common::MessageScheduleLoginSync *)> JoinScheduleCallback;
  typedef boost::function<void (const common::MessageScheduleLogoutSync *)> DisjoinScheduleCallback;
 public:
  ScheduleClient();
  virtual ~ScheduleClient();

  virtual void OnTimer(core::uint32 id);

  bool Initialize(core::EventLoop *loop, core::uint32 app_server_id,
      common::ServerType::type app_server_type, const core::InetAddress &local_address,
      core::uint32 schedule_id, const core::InetAddress &schedule_address);

  void Start();
  void Stop();

  inline void SetJoinScheduleCallback(const JoinScheduleCallback &callback) {
    this->join_schedule_callback_ = callback;
  }
  inline void SetDisjoinScheduleCallback(const DisjoinScheduleCallback &callback) {
    this->disjoin_schedule_callback_ = callback;
  }
  inline void SetCurrentSession(core::uint32 id, const ScheduleClientSessionPtr &session) {
    this->current_session_id_ = id;
    this->current_session_ = session;
  }

  inline common::ServerType::type GetHostType() const { return this->host_type_; }
  inline core::uint32 GetHostID() const { return this->host_id_; }
  inline const std::string &GetHostIP() const { return this->host_ip_; }
  inline core::uint16 GetHostPort() const { return this->host_port_; }

 private:
  void OnConnection(const core::TCPConnectionPtr &connection);
  void OnLoginScheduleSync(const common::MessageScheduleLoginSync *message);
  void OnLogoutScheduleSync(const common::MessageScheduleLogoutSync *message);

  void OnDisconnected();
  void OnConnectTimeout();
  void ResetNextNode();

  core::EventLoop *loop_;

  // Host type, id, ip and port.
  common::ServerType::type host_type_;
  core::uint32 host_id_;
  std::string host_ip_;
  core::uint16 host_port_;

  core::uint32 default_session_id_;
  core::uint32 current_session_id_;
  ScheduleClientSessionPtr current_session_;
  ClientSessionMap sessions_;

  // Any server except schedule server login/logout schedule type server,
  // this 2 callbacks will be invoke.
  JoinScheduleCallback join_schedule_callback_;
  DisjoinScheduleCallback disjoin_schedule_callback_;
};

class ScheduleClientSingleton : public core::Noncopyable {
 public:
   ScheduleClientSingleton() {}
   ~ScheduleClientSingleton() {}

   static inline ScheduleClient *GetScheduleClient() {
     return client_;
   }
   static inline void SetScheduleClient(ScheduleClient *client) {
     client_ = client;
   }

 private:
   static ScheduleClient *client_;
};

}  // namespace client

}  // namespace schedule

}  // namespace squirrel

#endif  // _squirrel_schedule_client__SCHEDULE__CLIENT__H

