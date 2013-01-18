//
// Summary: server core source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-08-27 18:48:14.
// File name: schedule_server.h
//
// Description: 
// Define class ScheduleServer.
//

#ifndef _squirrel_schedule_server__SCHEDULE__SERVER__H
#define _squirrel_schedule_server__SCHEDULE__SERVER__H

#include <boost/shared_ptr.hpp>

#include "core/base/noncopyable.h"
#include "core/event_loop_thread_group.h"
#include "core/tcp_connection.h"
#include "core/tcp_server.h"
#include "schedule_server/server/server_configure.h"
#include "schedule_server/server/schedule_circle.h"
#include "schedule_server/server/schedule_terminal_set.h"
#include "global/thrift_packet.h"

namespace squirrel {

namespace schedule {

namespace server {

class ScheduleServer : public core::Noncopyable {
 typedef boost::shared_ptr<core::EventLoopThreadGroup> EventLoopThreadGroupPtr;
 public:
  ScheduleServer(const core::InetAddress &address, const std::string &name);
  ~ScheduleServer();

  bool Initialize(const ServerConfigure &configure, const std::string &log);

  void Start();
  void Stop();

  inline core::uint32 GetServerID() const { return this->server_id_; }
  inline ScheduleCircle &GetScheduleCircle() { return this->client_circle_; }

  inline const core::InetAddress &GetLocalAddress() const { return this->server_.GetAddress(); }

 private:
  inline bool StartAsyncLogging(const ServerConfigure &configure, const std::string &log);

  void OnConnection(const core::TCPConnectionPtr &connection);
  void OnMessage(const core::TCPConnectionPtr &connection,
      core::ByteBufferDynamic &buffer, const core::TimeTick &time_tick);
  void OnWriteComplete(const core::TCPConnectionPtr &connection);

  core::uint32 server_id_;

  EventLoopThreadGroupPtr thread_pool_;
  core::TCPServer server_;
  ScheduleTerminalSet schedule_terminal_set_;

  global::ThriftPacket packet_;

  ScheduleCircle client_circle_;
};

class ScheduleServerSingleton : public core::Noncopyable {
 public:
  ScheduleServerSingleton() {}
  ~ScheduleServerSingleton() {}

  static inline ScheduleServer *GetScheduleServer() {
    return server_;
  }
  static inline void SetScheduleServer(ScheduleServer *server) {
    server_ = server;
  }

 private:
  static ScheduleServer *server_;
};

}  // namespace server

}  // namespace schedule

}  // namespace squirrel

#endif  // _squirrel_schedule_server__SCHEDULE__SERVER__H

