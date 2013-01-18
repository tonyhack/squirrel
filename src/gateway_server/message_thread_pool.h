//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-11 15:13:50.
// File name: message_thread_pool.h
//
// Description: 
// Define class MessageThreadPool.
//

#ifndef _squirrel_gateway__MESSAGE__THREAD__POOL__H
#define _squirrel_gateway__MESSAGE__THREAD__POOL__H

#include <map>

#include "core/event_loop_thread_group.h"
#include "core/base/noncopyable.h"
#include "global/thrift_packet.h"

namespace squirrel {

namespace gateway {

class MessageThreadPool : public core::Noncopyable {
  typedef std::map<const core::EventLoop *, global::ThriftPacketPtr> PacketMap;
 public:
  MessageThreadPool();
  ~MessageThreadPool();

  bool Initialize(size_t thread_number, const std::string &name, bool timer = false);

  void Start();
  void Stop();

  inline core::EventLoop *GetNextLoop() {
    return this->thread_pool_.GetNextLoop();
  }

  inline global::ThriftPacketPtr GetPacket(const core::EventLoop *loop) {
    PacketMap::iterator iterator = this->packets_.find(loop);
    if(iterator != this->packets_.end()) {
      return iterator->second;
    } else {
      return global::ThriftPacketPtr();
    }
  }

  inline core::EventLoopThreadGroup *GetEventLoopThreadGroup() {
    return &this->thread_pool_;
  }

 private:
  core::EventLoopThreadGroup thread_pool_;

  PacketMap packets_;
};

}  // namespace gateway

}  // namespace squirrel

#endif  // _squirrel_gateway__MESSAGE__THREAD__POOL__H

