//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-07 20:30:08.
// File name: request_service.h
//
// Description: 
// Define class RequestService.
//

#ifndef _squirrel_dataproxy_server__REQUEST__SERVICE__H
#define _squirrel_dataproxy_server__REQUEST__SERVICE__H

#include "common/message_data_proxy_types.h"
#include "core/base/types.h"
#include "core/base/noncopyable.h"
#include "data_proxy_server/redis_proxy/redis_reconnector.h"
#include "data_proxy_server/storage_proxy/storage_reconnector.h"
#include "data_proxy_server/server/async_request_pool.h"
#include "global/time_consuming.h"

namespace squirrel {

namespace dataproxy {

namespace server {

class RequestService : public core::Noncopyable {
 public:
  RequestService();
  ~RequestService();

  bool Initialize(size_t max_buffer_size, core::uint32 read_thread_number,
      core::uint32 write_thread_number, const RedisConfigure &redis_configure,
      const StorageConfigure &storage_configure, const ProcedureConfigure &procedure_configure);

  void Start();
  void Stop();

  bool Request(common::RequestRWType::type type,
      const common::AsyncRequestObject *object);

  inline void ReconnectRedisServer(const redis::RedisPtr &redis) {
    redis::RedisReconnectRequest reconnect(redis);
    this->redis_reconnector_.Reconnect(reconnect);
  }

  inline void ReconnectStorageServer(const storage::StoragePtr &storage) {
    storage::StorageReconnectRequest reconnect(storage);
    this->storage_reconnector_.Reconnect(reconnect);
  }

#ifdef _TIME_CONSUMING_TEST
  global::TimeConsuming request_consume_;
#endif

 private:
  AsyncRequestPool read_pool_;
  AsyncRequestPool write_pool_;

  redis::RedisReconnector redis_reconnector_;
  storage::StorageReconnector storage_reconnector_;
};

class RequestServiceSingleton : public core::Noncopyable {
 public:
  RequestServiceSingleton() {}
  ~RequestServiceSingleton() {}

  static inline RequestService *GetRequestService() {
    return service_;
  }
  static inline void SetRequestService(RequestService *service) {
    service_ = service;
  }

 private:
  static RequestService *service_;
};

}  // namespace server

}  // namespace dataproxy

}  // namespace squirrel

#endif  // _squirrel_dataproxy_server__REQUEST__SERVICE__H

