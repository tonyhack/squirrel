//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-04 14:41:05.
// File name: async_request_loop.cc
//
// Description: 
// Define class AsyncRequestLoop.
//

#include "data_proxy_server/server/async_request_loop.h"

#include "common/async_request_object_types.h"
#include "data_proxy_server/server/async_request.h"
#include "data_proxy_server/server/request_service.h"
#include "global/async_logging_singleton.h"

namespace squirrel {

namespace dataproxy {

namespace server {

AsyncRequestLoop::AsyncRequestLoop() {}
AsyncRequestLoop::~AsyncRequestLoop() {}

bool AsyncRequestLoop::Initialize(size_t max_buffer_size,
    const AsyncRequestPtr &async_request, const RedisConfigure &redis_configure,
    const StorageConfigure &storage_configure, const ProcedureConfigure &procedure_configure) {
  if(this->keyvalue_handler_.Initialize(this, redis_configure,
        storage_configure, procedure_configure) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [AsyncRequestLoop] Initialize KeyvalueRequestHandler failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  if(this->number_handler_.Initialize(this, redis_configure,
        storage_configure, procedure_configure) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [AsyncRequestLoop] Initialize NumberRequestHandler failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  if(this->list_handler_.Initialize(this, redis_configure,
        storage_configure, procedure_configure) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [AsyncRequestLoop] Initialize ListRequestHandler failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  if(this->map_handler_.Initialize(this, redis_configure,
        storage_configure, procedure_configure) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [AsyncRequestLoop] Initialize MapRequestHandler failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  if(this->default_handler_.Initialize(this) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [AsyncRequestLoop] Initialize DefaultRequestHandler failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  this->async_request_ = async_request;
  core::BoundQueue *queue = new (std::nothrow) core::BoundQueue(
      max_buffer_size, core::kBlockingAllPattern);
  if(queue) {
    this->queue_.reset(queue);
    return true;
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [AsyncRequestLoop] Allocate BoundQueue error.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
}

void AsyncRequestLoop::Loop() {
  this->looping_ = true;
  int ret = 0;

  while(this->quit_ == false) {
    ret = this->queue_->Remove(this->buffer_, kBufferSize);
    if(ret < 0 && this->quit_ == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [AsyncRequestLoop] BoundQueue::Remove failed [%d].",
          __FILE__, __LINE__, __FUNCTION__, ret);
      break;
    } else {
      this->HandleRequest(this->buffer_, ret);
    }
  }

  this->looping_ = false;
}

void AsyncRequestLoop::Wakeup() {
  this->queue_->Terminate();
}

bool AsyncRequestLoop::Request(const char *request, size_t size) {
  if(this->looping_ == true) {
    int ret = 0;
    ret = this->queue_->Add(request, size);
    if(ret != (int)size) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [AsyncRequestLoop] Request failed, BoundQueue::Add failed [%d].",
          __FILE__, __LINE__, __FUNCTION__, ret);
      return false;
    }
    return true;
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [AsyncRequestLoop] Request failed, thread is not looping.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
}

void AsyncRequestLoop::SendResponseFailure(core::uint64 request_id,
    core::uint64 terminal_guid, const std::string &key) {
  common::MessageDataProxyDataResponse message;
  message.__set_request_id_(request_id);
  message.__set_key_(key);
  message.__set_type_(common::RequestType::REQUEST_TYPE_MAX);
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
      "%s:%d (%s) [20120918] SendResponse [%d] is NULL.",
      __FILE__, __LINE__, __FUNCTION__, request_id);
  size_t size = 0;
  const char *msg_ptr = this->packet_.Serialize<common::MessageDataProxyDataResponse>(&message, size);
  if(msg_ptr) {
    common::MessageDataProxy message_dataproxy;
    message_dataproxy.__set_type_(common::MessageDataProxyType::MESSAGE_DATA_PROXY_TYPE_DATA_RESPONSE);
    message_dataproxy.__isset.message_ = true;
    message_dataproxy.message_.assign(msg_ptr, size);
    size = 0;
    const char *msg_ptr = this->packet_.Serialize<common::MessageDataProxy>(
        &message_dataproxy, size);
    if(msg_ptr) {
      common::Message msg;
      msg.__set_type_(common::MessageType::MESSAGE_TYPE_DATA_PROXY);
      msg.message_.assign(msg_ptr, size);
      msg_ptr = this->packet_.Serialize<common::Message>(&msg, size);
      if(msg_ptr) {
        RequestTerminalPtr terminal =
          RequestTerminalSetSingleton::GetRequestTerminalSet().GetRequestTerminal(terminal_guid);
        if(terminal) {
          terminal->SendMessage(msg_ptr, size);
        } else {
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
              "%s:%d (%s) GetRequestTerminal[%ld] is NULL.", __FILE__, __LINE__, __FUNCTION__,
              terminal_guid);
        }
      } else {
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
            "%s:%d (%s) [RequestTerminal] Serialize Message failed.",
            __FILE__, __LINE__, __FUNCTION__);
      }
    } else {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [RequestTerminal] Serialize MessageDataProxy failed.",
          __FILE__, __LINE__, __FUNCTION__);
    }
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) Serialize MessageDataProxyDataResponse failed.",
        __FILE__, __LINE__, __FUNCTION__);
  }
}

void AsyncRequestLoop::HandleRequest(const char *request, size_t size) {
#ifdef _TIME_CONSUMING_TEST
  {
  global::TimeConsuming time_consume("AsyncRequestLoop::HandleRequest");
#endif
  common::AsyncRequestObject object;
  if(this->packet_.Deserialize<common::AsyncRequestObject>(
        &object, request, size) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [AsyncRequestLoop] Deserialize AsyncRequestObject failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return ;
  }
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
      "%s:%d (%s) [20120918] HandleRequest id[%d].",
      __FILE__, __LINE__, __FUNCTION__, object.request_id_);
  switch(object.type_) {
    case common::RequestType::REQUEST_TYPE_KEYVALUE:
      this->keyvalue_handler_.Request(&object);
      break;
    case common::RequestType::REQUEST_TYPE_NUMBER:
      this->number_handler_.Request(&object);
      break;
    case common::RequestType::REQUEST_TYPE_LIST:
      this->list_handler_.Request(&object);
      break;
    case common::RequestType::REQUEST_TYPE_MAP:
      this->map_handler_.Request(&object);
      break;
    default:
      this->default_handler_.Request(&object);
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [AsyncRequestLoop] Error type[%d] when handle request.",
          __FILE__, __LINE__, __FUNCTION__, object.type_);
      break;
  }
#ifdef _TIME_CONSUMING_TEST
  }
  RequestServiceSingleton::GetRequestService()->request_consume_.ConsumeLogging();
#endif
}

}  // namespace server

}  // namespace dataproxy

}  // namespace squirrel

