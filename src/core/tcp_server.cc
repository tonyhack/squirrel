//
// Summary: server core source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-08-27 15:47:39.
// File name: tcp_server.cc
//
// Description: 
// Define class TCPServer.
//

#include "core/tcp_server.h"

#include <boost/bind.hpp>

#include "core/acceptor.h"
#include "core/event_loop.h"
#include "core/event_loop_thread_group.h"
#include "core/tcp_connection.h"

namespace core {

TCPServer::TCPServer(const InetAddress &address, const std::string &name)
       : name_(name), loop_(NULL), acceptor_(NULL), local_address_(address) {}

TCPServer::~TCPServer() {
  this->RemoveAll();
}

bool TCPServer::Initialize(EventLoop *loop, EventLoopThreadGroup *thread_group) {
  assert(loop);
  assert(thread_group);
  this->loop_ = loop;
  this->thread_group_ = thread_group;
  this->acceptor_ = new (std::nothrow) Acceptor(this->local_address_, this->loop_);
  assert(this->acceptor_);
  this->acceptor_->SetNewConnectionCallback(boost::bind(&TCPServer::NewTCPConnection, this, _1, _2));
  return this->acceptor_->Initialize();
}

bool TCPServer::Start() {
  if(this->acceptor_->Listen() == false) {
     CoreLog(ERROR, "%s:%d (%s) Server[%s] Listen error!!",
            __FILE__, __LINE__, __FUNCTION__, this->name_.c_str());
    return false;
  }
  CoreLog(INFO, "%s:%d (%s) Server[%s] start!!",
         __FILE__, __LINE__, __FUNCTION__, this->name_.c_str());
  return true;
}

void TCPServer::Stop() {
}

void TCPServer::NewTCPConnection(int sockfd, const InetAddress &peer) {
  char buffer[16];
  snprintf(buffer, sizeof(buffer), ":%d", sockfd);
  std::string connection_name = this->name_ + buffer;
  TCPConnectionPtr connection(new (std::nothrow) TCPConnection(sockfd, this->thread_group_->GetNextLoop(),
         connection_name, this->local_address_, peer));
  if(connection == NULL) {
    CoreLog(ERROR, "%s:%d (%s) Server[%s] failed to allocate tcp connection.",
         __FILE__, __LINE__, __FUNCTION__, this->name_.c_str());
    return ;
  }
  if(connection->Initialize() == false) {
    CoreLog(ERROR, "%s:%d (%s) Server[%s] failed to initialize tcp connection.",
         __FILE__, __LINE__, __FUNCTION__, this->name_.c_str());
    return ;
  }
  connection->SetConnectionCallback(this->connection_callback_);
  connection->SetMessageCallback(this->message_callback_);
  connection->SetWriteCompleteCallback(this->write_complete_callback_);
  connection->SetCloseCallback(boost::bind(&TCPServer::RemoveTCPConnection, this, _1));
  this->connections_.insert(std::make_pair(connection_name, connection));
  connection->ConnectionEstablished();
}

void TCPServer::RemoveTCPConnection(const TCPConnectionPtr &connection) {
  this->loop_->WakeupCallback(boost::bind(&TCPServer::RemoveInThreadLoop, this, connection), true);
}

void TCPServer::RemoveInThreadLoop(const TCPConnectionPtr &connection) {
  assert(connection);
  ConnectionList::iterator iterator = this->connections_.find(connection->GetName());
  if(iterator != this->connections_.end()) {
    assert(iterator->second);
    iterator->second->ConnectionDestoryed();
    this->connections_.erase(iterator);
  } else {
    CoreLog(ERROR, "%s:%d (%s) Server[%s] failed to find connection[%s]",
           __FILE__, __LINE__, __FUNCTION__, this->name_.c_str(), connection->GetName().c_str());
  }
}

void TCPServer::RemoveAll() {
  ConnectionList::iterator iterator = this->connections_.begin();
  for(; iterator != this->connections_.end(); ++iterator) {
    assert(iterator->second);
    iterator->second->ConnectionDestoryed();
  }
  this->connections_.clear();
}

}  // namespace core

