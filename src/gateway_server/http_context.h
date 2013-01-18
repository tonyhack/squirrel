//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-10 20:20:24.
// File name: http_context.h
//
// Description: 
// Define class HttpContext.
//

#ifndef _squirrel_gateway__HTTP__CONTEXT__H
#define _squirrel_gateway__HTTP__CONTEXT__H

#include "gateway_server/http_request.h"

namespace squirrel {

namespace gateway {

class HttpContext {
 public:
  enum HttpRequestParseState {
    EXPECT_REQUEST_LINE = 0,
    EXPECT_HEADER,
    EXPECT_BODY,
    EXPECT_COMPLETE,
  };

  HttpContext() : state_(EXPECT_REQUEST_LINE), request_id_(0) {}

  inline bool CheckExpectRequestLine() const {
    return this->state_ == EXPECT_REQUEST_LINE;
  }
  inline bool CheckExpectHeader() const {
    return this->state_ == EXPECT_HEADER;
  }
  inline bool CheckExpectBody() const {
    return this->state_ == EXPECT_BODY;
  }
  inline bool CheckExpectAll() const {
    return this->state_ >= EXPECT_COMPLETE;
  }

  inline void ReceiveRequestLine() {
    this->state_ = EXPECT_HEADER;
  }
  inline void ReceiveHeader() {
    this->state_ = EXPECT_BODY;
  }
  inline void ReceiveBody() {
    this->state_ = EXPECT_COMPLETE;
  }
  inline void ReceiveAll() {
    this->state_ = EXPECT_COMPLETE;
  }

  void Reset() {
    this->state_ = EXPECT_REQUEST_LINE;
    request_.Reset();
    request_id_ = 0;
  }

  const HttpRequest &GetRequest() const { return this->request_; }
  HttpRequest &GetRequest() { return this->request_; }

  inline void SetRequestID(core::uint64 id) { this->request_id_ = id; }
  inline core::uint64 GetRequestID() const { return this->request_id_; }

 private:
  HttpRequestParseState state_;
  HttpRequest request_;
  core::uint64 request_id_;
};

}  // namespace gateway

}  // namespace squirrel

#endif  // _squirrel_gateway__HTTP__CONTEXT__H

