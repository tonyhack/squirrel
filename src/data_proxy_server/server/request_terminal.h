//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-03 20:57:15.
// File name: request_terminal.h
//
// Description: 
// Define class Terminal.
//

#ifndef _squirrel_dataproxy_server__REQUEST__TERMINAL__H
#define _squirrel_dataproxy_server__REQUEST__TERMINAL__H

#include <boost/enable_shared_from_this.hpp>

#include "core/base/noncopyable.h"
#include "common/message_data_proxy_types.h"
#include "common/server_type_types.h"
#include "core/tcp_connection.h"
#include "core/base/byte_buffer_dynamic.h"
#include "core/base/noncopyable.h"
#include "core/base/types.h"
#include "global/async_logging_singleton.h"
#include "global/thrift_packet.h"

namespace squirrel {

using namespace global;

namespace dataproxy {

namespace server {

class RequestTerminal : public core::Noncopyable,
  public boost::enable_shared_from_this<RequestTerminal> {
 public:
  RequestTerminal();
  ~RequestTerminal();

  inline void Initialize(const core::TCPConnectionPtr &connection) {
    this->connection_ = connection;
    // this->connection_->GetPeerAddress().GetAddressInfo(host_, port_);
  }

  inline core::uint64 GetServerGuid() const { return this->guid_; }
  inline void SetServerGuid(core::uint64 guid) { this->guid_ = guid; }

  inline common::ServerType::type GetServerType() const { return this->type_; }
  inline void SetServerType(common::ServerType::type type) { this->type_ = type; }

  inline core::uint32 GetServerID() const { return this->id_; }
  inline void SetServerID(core::uint32 id) { this->id_ = id; }

  inline const std::string &GetServerHost() const { return this->host_; }
  inline void SetServerHost(const std::string &host) { this->host_ = host; }

  inline core::uint16 GetServerPort() const { return this->port_; }
  inline void SetServerPort(core::uint16 port) { this->port_ = port; }

  inline bool CheckLoginState() const { return this->login_state_; }

  void OnMessage(const common::MessageDataProxy *message);

  void OnDisConnnect();

  void SendMessage(const char *message, size_t size);

 private:
  void Send(const char *message, size_t size);
  void Send(const common::MessageDataProxy *message);

  template <typename T>
  void Send(const T *message, common::MessageDataProxyType::type type) {
    size_t size = 0;
    const char *msg_ptr = this->packet_.Serialize<T>(message, size);
    if(msg_ptr) {
      common::MessageDataProxy message_dataproxy;
      message_dataproxy.__set_type_(type);
      message_dataproxy.__isset.message_ = true;
      message_dataproxy.message_.assign(msg_ptr, size);
      this->Send(&message_dataproxy);
    } else {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) Serialize MessageDataProxyXXX[type=%d] failed.",
          __FILE__, __LINE__, __FUNCTION__, type);
    }
  }

  void OnTerminalLogin(const char *message, size_t size);
  void OnTerminalLogout(const char *message, size_t size);
  void OnTerminalRequest(const char *message, size_t size);

  core::uint64 guid_;

  common::ServerType::type type_;
  core::uint32 id_;
  std::string host_;
  core::uint16 port_;

  core::TCPConnectionPtr connection_;
  global::ThriftPacket packet_;

  bool login_state_;

  core::SimpleByteBufferDynamic buffer_;

  core::Mutex mutex_;
  core::SimpleByteBufferDynamic mt_buffer_;
};

typedef boost::shared_ptr<RequestTerminal> RequestTerminalPtr;

}  // namespace server

}  // namespace dataproxy

}  // namespace squirrel

#endif  // _squirrel_dataproxy_server__REQUEST__TERMINAL__H

