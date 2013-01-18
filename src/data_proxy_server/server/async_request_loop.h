//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-04 13:57:21.
// File name: async_request_loop.h
//
// Description: 
// Define class AsyncRequestLoop.
//

#ifndef _squirrel_dataproxy_server__ASYNC__REQUEST__LOOP__H
#define _squirrel_dataproxy_server__ASYNC__REQUEST__LOOP__H

#include <boost/shared_ptr.hpp>

#include "common/message_types.h"
#include "core/base_loop.h"
#include "core/base/condition_variable.h"
#include "data_proxy_server/server/default_request_handler.h"
#include "data_proxy_server/server/keyvalue_request_handler.h"
#include "data_proxy_server/server/list_request_handler.h"
#include "data_proxy_server/server/number_request_handler.h"
#include "data_proxy_server/server/map_request_handler.h"
#include "data_proxy_server/server/request_terminal.h"
#include "data_proxy_server/server/request_terminal_set.h"
#include "global/thrift_packet.h"

namespace core {

class BoundQueue;

}  // namespace core

namespace squirrel {

namespace dataproxy {

namespace server {

class AsyncRequest;

typedef boost::shared_ptr<AsyncRequest> AsyncRequestPtr;

namespace {

const size_t kBufferSize = 64 * 1024;

}  // namespace

class AsyncRequestLoop : public core::BaseLoop {
  typedef boost::shared_ptr<core::BoundQueue> BoundQueuePtr;
 public:
  AsyncRequestLoop();
  virtual ~AsyncRequestLoop();

  bool Initialize(size_t max_buffer_size, const AsyncRequestPtr &async_request,
      const RedisConfigure &redis_configure, const StorageConfigure &storage_configure,
      const ProcedureConfigure &procedure_configure);

  inline AsyncRequestPtr GetAsyncRequest() {
    return this->async_request_;
  }

  virtual void Loop();
  virtual void Wakeup();

  bool Request(const char *request, size_t size);

  template <typename T>
  void SendResponse(core::uint64 request_id, core::uint64 terminal_guid,
      common::RequestType::type request_type, const std::string &key, const T *response);
  void SendResponseFailure(core::uint64 request_id, core::uint64 terminal_guid,
      const std::string &key);

 private:
  void HandleRequest(const char *request, size_t size);

  AsyncRequestPtr async_request_;

  BoundQueuePtr queue_;
  global::ThriftPacket packet_;
  char buffer_[kBufferSize];

  KeyvalueRequestHandler keyvalue_handler_;
  NumberRequestHandler number_handler_;
  ListRequestHandler list_handler_;
  MapRequestHandler map_handler_;
  DefaultRequestHandler default_handler_;
};

template <typename T>
void AsyncRequestLoop::SendResponse(core::uint64 request_id, core::uint64 terminal_guid,
    common::RequestType::type request_type, const std::string &key, const T *response) {
  size_t size = 0;
  const char *msg_ptr = this->packet_.Serialize<T>(response, size);
  if(msg_ptr) {
    common::MessageDataProxyDataResponse message;
    message.__set_request_id_(request_id);
    message.__set_type_(request_type);
    message.__set_key_(key);
    message.response_.assign(msg_ptr, size);
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
        "%s:%d (%s) [20120918] SendResponse [%d] is NULL.", __FILE__, __LINE__, __FUNCTION__,
        request_id);
    size = 0;
    msg_ptr = this->packet_.Serialize<common::MessageDataProxyDataResponse>(&message, size);
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
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) Serialize Response[type=%d] failed.",
        __FILE__, __LINE__, __FUNCTION__, request_type);
  }
}

}  // namespace server

}  // namespace dataproxy

}  // namespace squirrel

#endif  // _squirrel_dataproxy_server__ASYNC__REQUEST__LOOP__H

