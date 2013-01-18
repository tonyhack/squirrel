//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-04 13:51:48.
// File name: async_request.h
//
// Description: 
// Define class AsyncRequest.
//

#ifndef _squirrel_dataproxy_server__ASYNC__REQUEST__H
#define _squirrel_dataproxy_server__ASYNC__REQUEST__H

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include "common/async_request_object_types.h"
#include "core/loop_thread.h"
#include "core/base/mutex.h"
#include "core/base/noncopyable.h"
#include "data_proxy_server/server/async_request_loop.h"
#include "global/thrift_packet.h"

namespace squirrel {

namespace dataproxy {

namespace server {

class AsyncRequest : public core::Noncopyable,
  public boost::enable_shared_from_this<AsyncRequest> {
 typedef boost::shared_ptr<core::LoopThread> LoopThreadPtr;
 public:
  AsyncRequest();
  ~AsyncRequest();

  bool Initialize(size_t max_buffer_size, const RedisConfigure &redis_configure,
      const StorageConfigure &storage_configure, const ProcedureConfigure &procedure_configure);

  bool Request(const common::AsyncRequestObject *object);

  void Start();
  void Stop();

 private:
  AsyncRequestLoop loop_;
  LoopThreadPtr loop_thread_;

  global::ThriftPacket packet_;
  core::Mutex mutex_;
};

typedef boost::shared_ptr<AsyncRequest> AsyncRequestPtr;

}  // namespace server

}  // namespace dataproxy

}  // namespace squirrel

#endif  // _squirrel_dataproxy_server__ASYNC__REQUEST__H

