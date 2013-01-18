//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-07 18:56:44.
// File name: async_request_pool.h
//
// Description: 
// Define class AsyncRequestPool.
//

#ifndef _squirrel_dataproxy_server__ASYNC__REQUEST__POOL__H
#define _squirrel_dataproxy_server__ASYNC__REQUEST__POOL__H

#include "core/base/mutex.h"
#include "core/base/noncopyable.h"
#include "data_proxy_server/server/async_request.h"

namespace squirrel {

namespace dataproxy {

namespace server {

class AsyncRequestPool : public core::Noncopyable {
  typedef std::vector<AsyncRequestPtr> AsyncRequestVec;
 public:
  AsyncRequestPool();
  ~AsyncRequestPool();

  bool Initialize(size_t max_buffer_size, core::uint32 thread_number,
      const RedisConfigure &redis_configure, const StorageConfigure &storage_configure,
      const ProcedureConfigure &procedure_configure);

  bool Request(const common::AsyncRequestObject *object);

  void Start();
  void Stop();

 private:
  AsyncRequestVec async_requests_;
};

}  // namespace server

}  // namespace dataproxy

}  // namespace squirrel

#endif  // _squirrel_dataproxy_server__ASYNC__REQUEST__POOL__H

