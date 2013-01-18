//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-29 08:03:50.
// File name: default_request_handler.h
//
// Description: 
// Define class DefaultRequestHandler
//

#ifndef _squirrel_dataproxy_server__DEFAULT__REQUEST__HANDLER__H
#define _squirrel_dataproxy_server__DEFAULT__REQUEST__HANDLER__H

#include "common/map_message_types.h"
#include "common/async_request_object_types.h"
#include "core/base/noncopyable.h"
#include "global/thrift_packet.h"

namespace squirrel {

namespace dataproxy {

namespace server {

class AsyncRequestLoop;

class DefaultRequestHandler : public core::Noncopyable {
 public:
  DefaultRequestHandler();
  ~DefaultRequestHandler();

  bool Initialize(AsyncRequestLoop *async_request_loop_ptr);

  void Request(const common::AsyncRequestObject *object);

 private:
  global::ThriftPacket packet_;
  AsyncRequestLoop *async_request_loop_;
};

}  // namespace server

}  // namespace dataproxy

}  // namespace squirrel

#endif  // _squirrel_dataproxy_server__DEFAULT__REQUEST__HANDLER__H

