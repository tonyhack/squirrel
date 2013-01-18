//
// Summary: server core source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify:	2012-08-27 15:35:12.
// File name:	acceptor.h
//
// Description: 
// Define class Acceptor.
// This acceptor uses non-blocking pattern.
//

#include "core/acceptor.h"

#include <boost/bind.hpp>

#include "core/channel.h"
#include "core/inet_address.h"
#include "core/base/logging.h"

namespace core {

Acceptor::Acceptor(const InetAddress &listen_address, EventLoop *loop)
       : local_address_(listen_address), loop_(loop), channel_(NULL) {}

Acceptor::~Acceptor() {
  if(this->channel_)
    delete this->channel_;
}

bool Acceptor::Initialize() {
  if(this->socket_.CreateSocket() == false ||
         this->socket_.SetNonblock() == false ||
         this->socket_.Bind(this->local_address_) == false) {
    CoreLog(ERROR, "%s:%d (%s) Failed to create socket or bind address.",
           __FILE__, __LINE__, __FUNCTION__);
    return false;
  } else {
    this->channel_ = new (std::nothrow) Channel(this->loop_, this->socket_.GetSockfd());
    if(this->channel_ == NULL) {
      CoreLog(ERROR, "%s:%d (%s) Failed to allocate a channel_",
             __FILE__, __LINE__, __FUNCTION__);
      return false;
    } else {
      this->channel_->SetReadCallback(boost::bind(&Acceptor::Accept, this, _1));
      return true;
    }
  }
}

bool Acceptor::Listen() {
  if(this->socket_.Listen(SOMAXCONN) == true) {
    assert(this->channel_);
    this->channel_->EnableRead();
    return true;
  }
  return false;
}

void Acceptor::Accept(const TimeTick &time_tick) {
  CoreLog(INFO, "Acceptor::Accept()");
  InetAddress peer;
  int peer_sockfd = this->socket_.Accept(peer);
  if(peer_sockfd >= 0) {
    if(this->callback_)
      this->callback_(peer_sockfd, peer);
    else
      ::close(peer_sockfd);
  } else {
    CoreLog(ERROR, "%s:%d (%s) Failed to accept",
             __FILE__, __LINE__, __FUNCTION__);
  }
}

}  // namespace core

