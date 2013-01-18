//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-21 20:35:54.
// File name: list_request_handler.h
//
// Description: 
// Define class ListRequestHandler.
//

#ifndef _squirrel_dataproxy_server__LIST__REQUEST__HANDLER__H
#define _squirrel_dataproxy_server__LIST__REQUEST__HANDLER__H

#include "common/async_request_object_types.h"
#include "core/base/noncopyable.h"
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

class ListRequestHandler : public core::Noncopyable {
 public:
  ListRequestHandler();
  ~ListRequestHandler();

  bool Initialize(AsyncRequestLoop *async_request_loop_ptr,
      const RedisConfigure &redis_configure, const StorageConfigure &storage_configure,
      const ProcedureConfigure &procedure_configure);

  void Request(const common::AsyncRequestObject *object);

 private:
  void OnRequestPush(const common::AsyncRequestObject *object,
      const std::string &element);
  void OnRequestPop(const common::AsyncRequestObject *object);
  void OnRequestRemove(const common::AsyncRequestObject *object,
      const std::string &element);
  void OnRequestClear(const common::AsyncRequestObject *object);
  void OnRequestDelete(const common::AsyncRequestObject *object);
  void OnRequestDefault(const common::AsyncRequestObject *object);

  global::ThriftPacket packet_;
  redis::RedisSet redises_;
  storage::StorageSet storages_;

  AsyncRequestLoop *async_request_loop_;

};

}  // namespace server

}  // namespace dataproxy

}  // namespace squirrel

#endif  // _squirrel_dataproxy_server__LIST__REQUEST__HANDLER__H

