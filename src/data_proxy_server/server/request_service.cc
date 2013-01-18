//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-07 20:36:25.
// File name: request_service.cc
//
// Description: 
// Define class RequestService.
//

#include "data_proxy_server/server/request_service.h"

#include "global/async_logging_singleton.h"

namespace squirrel {

using namespace global;

namespace dataproxy {

namespace server {

RequestService *RequestServiceSingleton::service_;

RequestService::RequestService() {}
RequestService::~RequestService() {}

bool RequestService::Initialize(size_t max_buffer_size,
    core::uint32 read_thread_number, core::uint32 write_thread_number,
    const RedisConfigure &redis_configure, const StorageConfigure &storage_configure,
    const ProcedureConfigure &procedure_configure) {
  if(this->read_pool_.Initialize(max_buffer_size, read_thread_number,
        redis_configure, storage_configure, procedure_configure) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [RequestService] Initialize AsyncRequestPool failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  if(this->write_pool_.Initialize(max_buffer_size, write_thread_number,
        redis_configure, storage_configure, procedure_configure) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [RequestService] Initialize AsyncRequestPool failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  return true;
}

void RequestService::Start() {
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[RequestService] RequestService start.");
  this->read_pool_.Start();
  this->write_pool_.Start();
  this->redis_reconnector_.Start();
  this->storage_reconnector_.Start();
}

void RequestService::Stop() {
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "[RequestService] RequestService start.");
  this->write_pool_.Stop();
  this->read_pool_.Stop();
  this->redis_reconnector_.Stop();
  this->storage_reconnector_.Stop();
}

bool RequestService::Request(common::RequestRWType::type type,
    const common::AsyncRequestObject *object) {
  if(type == common::RequestRWType::REQUEST_RW_TYPE_READ) {
    return this->read_pool_.Request(object);
  } else if(type == common::RequestRWType::REQUEST_RW_TYPE_WRITE) {
    return this->write_pool_.Request(object);
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [RequestService] RequestRWType error.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
}

}  // namespace server

}  // namespace dataproxy

}  // namespace squirrel

