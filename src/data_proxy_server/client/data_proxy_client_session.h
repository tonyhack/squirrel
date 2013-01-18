//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-10 10:39:49.
// File name: data_proxy_client_session.h
//
// Description: 
// Define class DataProxyClientSession.
//

#ifndef _squirrel_dataproxy_client__DATA__PROXY__CLIENT__SESSION__H
#define _squirrel_dataproxy_client__DATA__PROXY__CLIENT__SESSION__H

#include <boost/shared_ptr.hpp>

#include "common/message_data_proxy_types.h"
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

namespace dataproxy {

namespace client {

class DataProxyClientSession : public core::Noncopyable,
  public boost::enable_shared_from_this<DataProxyClientSession>{
 public:
  DataProxyClientSession(const core::InetAddress &server_address,
      const std::string &name);
  ~DataProxyClientSession();

  bool Initialize(core::EventLoop *loop, core::uint32 server_id);

  bool Start();
  void Stop();

  void SendMessage(const char *message, size_t size);

  inline bool CheckLoginState() const { return this->login_server_; }

  inline const core::InetAddress &GetServerAddress() const {
    return this->tcp_client_.GetServerAddress();
  }

  inline core::uint32 GetServerID() const {
    return this->server_id_;
  }

 private:
  void Send(const char *message, size_t size);
  void Send(const common::MessageDataProxy *message);

  template <typename T>
  void Send(const T *message, common::MessageDataProxyType::type type) {
    size_t size = 0;
    const char *msg_ptr = this->packet_.Serialize<T>(message, size);
    if(msg_ptr) {
      common::MessageDataProxy msg;
      msg.__set_type_(type);
      msg.__isset.message_ = true;
      msg.message_.assign(msg_ptr, size);
      this->Send(&msg);
    } else {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) Serialize MessageDataProxyXXX[type=%d] failed.",
          __FILE__, __LINE__, __FUNCTION__, type);
    }
  }

  void OnConnection(const core::TCPConnectionPtr &connection);
  void OnMessage(const core::TCPConnectionPtr &connection,
      core::ByteBufferDynamic &buffer, const core::TimeTick &time_tick);
  void OnWriteComplete(const core::TCPConnectionPtr &connection);

  void LoginServer();

  void DispatchMessage(const common::MessageDataProxy *message);

  // Login/Logout server response.
  void OnLoginServerResponse(const char *message, size_t size);
  void OnLogoutServerResponse(const char *message, size_t size);

  void OnResponse(const char *message, size_t size);

  core::uint32 server_id_;

  core::TCPClient tcp_client_;
  core::EventLoop *loop_;

  global::ThriftPacket packet_;
  core::SimpleByteBufferDynamic buffer_;

  core::TCPConnectionPtr connection_;
  core::ConnectionCallback connection_callback_;

  bool login_server_;

  core::Mutex mutex_;
  core::SimpleByteBufferDynamic mt_buffer_;
};

typedef boost::shared_ptr<DataProxyClientSession> DataProxyClientSessionPtr;

}  // namespace client

}  // namespace dataproxy

}  // namespace squirrel

#endif  // _squirrel_dataproxy_client__DATA__PROXY__CLIENT__SESSION__H

