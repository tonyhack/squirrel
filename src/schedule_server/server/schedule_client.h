//
// Summary: server core source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-08-27 18:36:45.
// File name: schedule_client.h
//
// Description: 
// Define class ScheduleClient.
//

#ifndef _squirrel_schedule_server__SCHEDULE__CLIENT__H
#define _squirrel_schedule_server__SCHEDULE__CLIENT__H

#include <boost/shared_ptr.hpp>

#include "common/message_schedule_types.h"
#include "core/callbacks.h"
#include "core/inet_address.h"
#include "core/tcp_client.h"
#include "core/tcp_connection.h"
#include "core/base/byte_buffer_dynamic.h"
#include "core/base/noncopyable.h"
#include "global/async_logging_singleton.h"
#include "global/thrift_packet.h"

namespace squirrel {

using namespace global;

namespace schedule {

namespace server {

class ScheduleClient : public core::Noncopyable {
 public:
  ScheduleClient(const core::InetAddress &server_address, const std::string &name);
  ~ScheduleClient();

  bool Initialize(core::EventLoop *loop);

  void Start();
  void Stop();

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

  inline void SetConnectionCallback(const core::ConnectionCallback &callback) {
    this->connection_callback_ = callback;
  }
  inline void ResetConnectionCallback() { this->connection_callback_.clear(); }

  inline bool CheckLoginState() const { return this->login_schedule_; }

  inline const core::InetAddress &GetServerAddress() const { return this->tcp_client_.GetServerAddress(); }

 private:
  void OnConnection(const core::TCPConnectionPtr &connection);
  void OnMessage(const core::TCPConnectionPtr &connection,
      core::ByteBufferDynamic &buffer, const core::TimeTick &time_tick);
  void OnWriteComplete(const core::TCPConnectionPtr &connection);

  void LoginScheduleServer();
  void LogoutScheduleServer();

  void DispatchMessage(const common::MessageSchedule *message);
  void OnLoginScheduleResponse(const char *message, size_t size);
  void OnLogoutScheduleResponse(const char *message, size_t size);

  core::TCPClient tcp_client_;
  core::EventLoop *loop_;

  global::ThriftPacket packet_;
  core::SimpleByteBufferDynamic buffer_;

  core::TCPConnectionPtr connection_;
  core::ConnectionCallback connection_callback_;

  bool login_schedule_;
};

typedef boost::shared_ptr<ScheduleClient> ScheduleClientPtr;

}  // namespace server

}  // namespace schedule

}  // namespace squirrel

#endif  // _squirrel_schedule_server__SCHEDULE__CLIENT__H

