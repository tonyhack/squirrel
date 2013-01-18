//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-11 15:41:38.
// File name: message_thread_pool.cc
//
// Description: 
// Define class MessageThreadPool.
//

#include "gateway_server/message_thread_pool.h"

#include "global/async_logging_singleton.h"

namespace squirrel {

using namespace global;

namespace gateway {

MessageThreadPool::MessageThreadPool() {}
MessageThreadPool::~MessageThreadPool() {}

bool MessageThreadPool::Initialize(size_t thread_number,
    const std::string &name, bool timer) {
  if(this->thread_pool_.Initialize(thread_number, name, timer) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MessageThreadPool] Initialize EventLoopThreadGroup error.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  const core::EventLoopThreadGroup::LoopGroup *group = this->thread_pool_.GetLoopGroup();
  if(group == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MessageThreadPool] GetLoopGroup error.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  core::EventLoopThreadGroup::LoopGroup::const_iterator iterator = group->begin();
  for(; iterator != group->end(); ++iterator) {
    // ThriftPacket.
    global::ThriftPacketPtr packet(new (std::nothrow) global::ThriftPacket());
    if(packet == NULL) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [MessageThreadPool] Allocate ThriftPacket error.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    this->packets_.insert(std::make_pair(*iterator, packet));
  }

  return true;
}

void MessageThreadPool::Start() {
  this->thread_pool_.Start();
}

void MessageThreadPool::Stop() {
  this->thread_pool_.Stop();
}

}  // namespace gateway

}  // namespace squirrel

