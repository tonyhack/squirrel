//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-07 19:05:04.
// File name: async_request_pool.cc
//
// Description: 
// Define class AsyncRequestPool.
//

#include "data_proxy_server/server/async_request_pool.h"

#include "global/async_logging_singleton.h"

namespace squirrel {

using namespace global;

namespace dataproxy {

namespace server {

AsyncRequestPool::AsyncRequestPool() {}
AsyncRequestPool::~AsyncRequestPool() {}

bool AsyncRequestPool::Initialize(size_t max_buffer_size, core::uint32 thread_number,
    const RedisConfigure &redis_configure, const StorageConfigure &storage_configure,
    const ProcedureConfigure &procedure_configure) {
  size_t pos = 0;
  for(; pos < thread_number; ++pos) {
    AsyncRequestPtr request(new (std::nothrow) AsyncRequest());
    if(request == NULL) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [AsyncRequestPool] Allocate AsyncRequest failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    if(request->Initialize(max_buffer_size, redis_configure,
          storage_configure, procedure_configure) == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [AsyncRequestPool] Initialize AsyncRequest failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    this->async_requests_.push_back(request);
  }
  return true;
}

bool AsyncRequestPool::Request(const common::AsyncRequestObject *object) {
  return this->async_requests_[object->key_hash_value_ %
    this->async_requests_.size()]->Request(object);
}

void AsyncRequestPool::Start() {
  AsyncRequestVec::iterator iterator = this->async_requests_.begin();
  for(; iterator != this->async_requests_.end(); ++iterator) {
    (*iterator)->Start();
  }
}

void AsyncRequestPool::Stop() {
  AsyncRequestVec::iterator iterator = this->async_requests_.begin();
  for(; iterator != this->async_requests_.end(); ++iterator) {
    (*iterator)->Stop();
  }
}

}  // namespace server

}  // namespace dataproxy

}  // namespace squirrel

