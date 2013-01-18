//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-29 08:07:12.
// File name: default_request_handler.cc
//
// Description: 
// Define class DefaultRequestHandler.
//

#include "data_proxy_server/server/default_request_handler.h"

#include "data_proxy_server/server/async_request_loop.h"
#include "data_proxy_server/server/request_service.h"
#include "global/async_logging_singleton.h"
#include "global/time_consuming.h"

namespace squirrel {

namespace dataproxy {

namespace server {

DefaultRequestHandler::DefaultRequestHandler() : async_request_loop_(NULL) {}
DefaultRequestHandler::~DefaultRequestHandler() {}

bool DefaultRequestHandler::Initialize(AsyncRequestLoop *async_request_loop_ptr) {
  this->async_request_loop_ = async_request_loop_ptr;
  return true;
}

void DefaultRequestHandler::Request(const common::AsyncRequestObject *object) {
  this->async_request_loop_->SendResponseFailure(object->request_id_,
      object->terminal_guid_, object->key_);
}

}  // namespace server

}  // namespace dataproxy

}  // namespace squirrel

