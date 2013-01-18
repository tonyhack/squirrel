//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-08-27 18:39:26.
// File name: schedule_terminal.h
//
// Description: 
// Define class ScheduleTerminal.
//

#ifndef _squirrel_schedule_server__SCHEDULE__TERMINAL__H
#define _squirrel_schedule_server__SCHEDULE__TERMINAL__H

#include <boost/enable_shared_from_this.hpp>

#include "common/message_schedule_types.h"
#include "common/server_type_types.h"
#include "core/tcp_connection.h"
#include "core/base/byte_buffer_dynamic.h"
#include "core/base/noncopyable.h"
#include "core/base/types.h"
#include "global/async_logging_singleton.h"
#include "global/thrift_packet.h"

namespace squirrel {

using namespace global;

namespace schedule {

namespace server {

class ScheduleTerminal : public core::Noncopyable,
  public boost::enable_shared_from_this<ScheduleTerminal> {
 public:
  ScheduleTerminal();
  ~ScheduleTerminal();

  inline void Initialize(const core::TCPConnectionPtr &connection) {
    this->connection_ = connection;
    // this->connection_->GetPeerAddress().GetAddressInfo(host_, port_);
  }

  inline common::ServerType::type GetServerType() const { return this->type_; }
  inline void SetServerType(common::ServerType::type type) { this->type_ = type; }

  inline core::uint32 GetServerID() const { return this->id_; }
  inline void SetServerID(core::uint32 id) { this->id_ = id; }

  inline const std::string &GetServerHost() const { return this->host_; }
  inline void SetServerHost(const std::string &host) { this->host_ = host; }

  inline core::uint16 GetServerPort() const { return this->port_; }
  inline void SetServerPort(core::uint16 port) { this->port_ = port; }

  inline bool CheckLoginState() const { return this->login_state_; }

  void Send(const char *message, size_t size);
  void Send(const common::MessageSchedule *message);

  template <typename T>
  void Send(const T *message, common::MessageScheduleType::type type) {
    size_t size = 0;
    const char *msg_ptr = this->packet_.Serialize<T>(message, size);
    if(msg_ptr) {
      common::MessageSchedule message_schedule;
      message_schedule.__set_type_(type);
      message_schedule.__isset.message_ = true;
      message_schedule.message_.assign(msg_ptr, size);
      this->Send(&message_schedule);
    } else {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) Serialize MessageScheduleXXX[type=%d] failed.",
          __FILE__, __LINE__, __FUNCTION__, type);
    }
  }

  void OnMessage(const common::MessageSchedule *message);

  void OnDisConnnect();

 private:
  void OnTerminalLogin(const char *message, size_t size);
  void OnTerminalLogout(const char *message, size_t size);
  void OnTerminalLoginSync(const char *message, size_t size);
  void OnTerminalLogoutSync(const char *message, size_t size);
  void OnTerminalTransferLoginSync(const char *message, size_t size);
  void OnTerminalTransferLogoutSync(const char *message, size_t size);

  common::ServerType::type type_;
  core::uint32 id_;
  std::string host_;
  core::uint16 port_;
  core::TCPConnectionPtr connection_;
  global::ThriftPacket packet_;
  core::SimpleByteBufferDynamic buffer_;

  bool login_state_;
};

typedef boost::shared_ptr<ScheduleTerminal> ScheduleTerminalPtr; 

}  // namespace server

}  // namespace schedule

}  // namespace squirrel

#endif  // _squirrel_schedule_server__SCHEDULE__TERMINAL__H

