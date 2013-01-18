//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-04 15:48:05.
// File name: keyvalue_request_handler.h
//
// Description: 
// Define class KeyvalueRequestHandler.
//

#ifndef _squirrel_dataproxy_server__KEYVALUE__REQUEST__HANDLER__H
#define _squirrel_dataproxy_server__KEYVALUE__REQUEST__HANDLER__H

#include "common/async_request_object_types.h"
#include "data_proxy_server/redis_proxy/redis_set.h"
#include "data_proxy_server/storage_proxy/storage_set.h"
#include "global/thrift_packet.h"

namespace squirrel {

using namespace global;

namespace dataproxy {

using redis::RedisConfigure;
using storage::StorageConfigure;
using storage::ProcedureConfigure;

namespace server {

class AsyncRequestLoop;

class KeyvalueRequestHandler : public core::Noncopyable {
 public:
  KeyvalueRequestHandler();
  ~KeyvalueRequestHandler();

  bool Initialize(AsyncRequestLoop *async_request_loop_ptr,
      const RedisConfigure &redis_configure, const StorageConfigure &storage_configure,
      const ProcedureConfigure &procedure_configure);

  void Request(const common::AsyncRequestObject *object);

 private:
  void OnRequestGet(const common::AsyncRequestObject *object);
  void OnRequestSet(const common::AsyncRequestObject *object, const std::string &value);
  void OnRequestDelete(const common::AsyncRequestObject *object);
  void OnRequestSynchronousRedis(const common::AsyncRequestObject *object);
  void OnRequestDefault(const common::AsyncRequestObject *object);

  void SynchronousRedis(const common::AsyncRequestObject *object);

  global::ThriftPacket packet_;
  redis::RedisSet redises_;
  storage::StorageSet storages_;

  AsyncRequestLoop *async_request_loop_;
};

}  // namespace server

}  // namespace dataproxy

}  // namespace squirrel

#endif  // _squirrel_dataproxy_server__KEYVALUE__REQUEST__HANDLER__H

