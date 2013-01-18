//
// Summary: server core source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-08-27 15:46:39.
// File name: tcp_client.h
//
// Description: 
// Define class TCPClient.
//

#ifndef __CORE__TCP__CLIENT__H
#define __CORE__TCP__CLIENT__H

#include <string>

#include "core/callbacks.h"
#include "core/configure.h"
#include "core/inet_address.h"
#include "core/tcp_connection.h"
#include "core/base/mutex.h"
#include "core/base/noncopyable.h"

namespace core {

class Connector;
class EventLoop;
// class TCPConnection;

class TCPClient : public Noncopyable {
 public:
  TCPClient(const InetAddress &server_address, const std::string &name);
  ~TCPClient();

  bool Initialize(EventLoop *loop);

  void Start();
  void Stop();

  inline void SetConnectionCallback(const ConnectionCallback &callback) {
    this->connection_callback_ = callback;
  }
  inline void SetMessageCallback(const MessageCallback &callback) {
    this->message_callback_ = callback;
  }
  inline void SetWriteCallback(const WriteCompleteCallback &callback) {
    this->write_complete_callback_ = callback;
  }

  inline const InetAddress &GetServerAddress() const { return this->server_address_; }

 private:
  void NewTCPConnection(int sockfd, const InetAddress &local);
  // void RemoveTCPConnection(const TCPConnection *connection);
  void RemoveTCPConnection(const TCPConnectionPtr &connection);

  bool connected_;
  bool retry_;

  std::string name_;
  EventLoop *loop_;
  Connector *connector_;
  // TCPConnection *connection_;
  TCPConnectionPtr connection_;
  InetAddress server_address_;

  mutable Mutex mutex_;

  // Callbacks.
  ConnectionCallback connection_callback_;
  MessageCallback message_callback_;
  WriteCompleteCallback write_complete_callback_;
};

}  // namespace core

#endif  // __CORE__TCP__CLIENT__H

