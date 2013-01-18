//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-04 14:08:10.
// File name: async_request.cc
//
// Description: 
// Define class AsyncRequest.
//

#include "data_proxy_server/server/async_request.h"

#include "global/async_logging_singleton.h"

namespace squirrel {

namespace dataproxy {

namespace server {

AsyncRequest::AsyncRequest() {}
AsyncRequest::~AsyncRequest() {}

bool AsyncRequest::Initialize(size_t max_buffer_size, const RedisConfigure &redis_configure,
      const StorageConfigure &storage_configure, const ProcedureConfigure& procedure_configure) {
  if(this->loop_.Initialize(max_buffer_size, this->shared_from_this(),
      redis_configure, storage_configure, procedure_configure)) {
    this->loop_thread_.reset(new (std::nothrow) core::LoopThread(
        &this->loop_, "AsyncRequest", true));
    if(this->loop_thread_ == NULL) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [AsyncRequest] Allocate LoopThread error.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    return true;
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [AsyncRequest] Initialize AsyncRequestLoop error.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
}

bool AsyncRequest::Request(const common::AsyncRequestObject *object) {
  size_t size = 0;
  // TODO: If invoker pass the string witch has been serialized, this mutex is not necessary.
  core::AutoMutex auto_mutex(&this->mutex_);
  const char *message = this->packet_.Serialize<common::AsyncRequestObject>(
      object, size);
  if(message) {
    return this->loop_.Request(message, size);
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [AsyncRequest] Serialize AsyncRequestObject failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
}

void AsyncRequest::Start() {
  this->loop_thread_->StartLoop();
}

void AsyncRequest::Stop() {
  this->loop_thread_->StopLoop(true);
}

}  // namespace server

}  // namespace dataproxy

}  // namespace squirrel

