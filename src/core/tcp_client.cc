//
// Summary: server core source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-08-27 15:46:50.
// File name: tcp_client.cc
//
// Description: 
// Define class TCPClient.
//

#include "core/tcp_client.h"

#include <boost/bind.hpp>

#include "core/configure.h"
#include "core/connector.h"
// #include "core/tcp_connection.h"
#include "core/base/auto_mutex.h"
#include "core/base/logging.h"

namespace core {

TCPClient::TCPClient(const InetAddress &server_address,
       const std::string &name)
       : connected_(false), retry_(true), name_(name),
         loop_(NULL), connector_(NULL), server_address_(server_address) {}

TCPClient::~TCPClient() {
  {
    AutoMutex auto_mutex(&this->mutex_);
    if(this->connection_) {
      this->connection_.reset();
    }
  }
  if(this->connector_) {
    delete this->connector_;
    this->connector_ = NULL;
  }
}

bool TCPClient::Initialize(EventLoop *loop) {
  assert(loop);
  this->loop_ = loop;
  this->connector_ = new (std::nothrow) Connector(this->loop_,
         this->server_address_, Configure::GetConnector()->retry_msec_);
  if(this->connector_) {
    this->connector_->SetNewConnectionCallback(
        boost::bind(&TCPClient::NewTCPConnection, this, _1, _2));
    return true;
  }
  return false;
}

void TCPClient::Start() {
  if(this->connected_ == false) {
    this->connector_->Start();
  }
}

void TCPClient::Stop() {
  this->retry_ = false;
  this->connector_->Stop();
  if(this->connected_ == true) {
    AutoMutex auto_mutex(&this->mutex_);
    if(this->connection_)
      this->connection_->Shutdown();
  }
}

void TCPClient::NewTCPConnection(int sockfd, const InetAddress &local) {
  char buffer[16];
  snprintf(buffer, sizeof(buffer), ":%d", sockfd);
  std::string connection_name = this->name_ + buffer;
  TCPConnection *connection = new (std::nothrow) TCPConnection(sockfd, this->loop_,
      connection_name, local, this->server_address_);
  if(connection == NULL) {
    CoreLog(ERROR, "%s:%d (%s) Client[%s] failed to allocate tcp connection.",
         __FILE__, __LINE__, __FUNCTION__, this->name_.c_str());
    return ;
  }
  if(connection->Initialize() == false) {
    CoreLog(ERROR, "%s:%d (%s) Client[%s] failed to initialize tcp connection.",
         __FILE__, __LINE__, __FUNCTION__, this->name_.c_str());
    delete connection;
    return ;
  }
  this->connection_.reset(connection);
  this->connection_->SetConnectionCallback(this->connection_callback_);
  this->connection_->SetMessageCallback(this->message_callback_);
  this->connection_->SetWriteCompleteCallback(this->write_complete_callback_);
  this->connection_->SetCloseCallback(boost::bind(&TCPClient::RemoveTCPConnection, this, _1));
  this->connection_->ConnectionEstablished();
  this->connected_ = true;
}

// void TCPClient::RemoveTCPConnection(const TCPConnection *connection) {
void TCPClient::RemoveTCPConnection(const TCPConnectionPtr &connection) {
  this->connected_ = false;
  assert(connection == this->connection_);
  {
    AutoMutex auto_mutex(&this->mutex_);
    this->connection_->ConnectionDestoryed();
    // delete this->connection_;
    // this->connection_ = NULL;
    this->connection_.reset();
  }
  if(this->retry_) {
    this->connector_->Restart();
  }
}

}  // namespace core

