//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-08-31 15:27:10.
// File name: schedule_client_session.h
//
// Description: 
// Define class ScheduleClientSession.
//

#ifndef _squirrel_schedule_client__SCHEDULE__CLIENT__SESSION__H
#define _squirrel_schedule_client__SCHEDULE__CLIENT__SESSION__H

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

namespace client {

class ScheduleClientSession : public core::Noncopyable {
 public:
  ScheduleClientSession(const core::InetAddress &server_address,
      const std::string &name);
  ~ScheduleClientSession();

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

  inline bool CheckLoginState() const { return this->login_schedule_; }

  inline const core::InetAddress &GetServerAddress() const {
    return this->tcp_client_.GetServerAddress();
  }

 private:
  void OnConnection(const core::TCPConnectionPtr &connection);
  void OnMessage(const core::TCPConnectionPtr &connection,
      core::ByteBufferDynamic &buffer, const core::TimeTick &time_tick);
  void OnWriteComplete(const core::TCPConnectionPtr &connection);

  void LoginScheduleServer();

  void DispatchMessage(const common::MessageSchedule *message);

  // Login/Logout schedule response.
  void OnLoginScheduleResponse(const char *message, size_t size);
  void OnLogoutScheduleResponse(const char *message, size_t size);

  // Synchronized login/logout schedule.
  void OnLoginScheduleSync(const char *message, size_t size);
  void OnLogoutScheduleSync(const char *message, size_t size);

  core::TCPClient tcp_client_;
  core::EventLoop *loop_;

  global::ThriftPacket packet_;
  core::SimpleByteBufferDynamic buffer_;

  core::TCPConnectionPtr connection_;
  core::ConnectionCallback connection_callback_;

  bool login_schedule_;
};

typedef boost::shared_ptr<ScheduleClientSession> ScheduleClientSessionPtr;

}  // namespace client

}  // namespace schedule

}  // namespace squirrel

#endif  // _squirrel_schedule_client__SCHEDULE__CLIENT__SESSION__H

