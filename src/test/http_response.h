//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-14 15:56:05.
// File name: http_response.h
//
// Description: 
// Define class HttpResponse.
//

#ifndef _squirrel_test__HTTP__RESPONSE__H
#define _squirrel_test__HTTP__RESPONSE__H

#include <stdio.h>
#include <string.h>

#include <map>
#include <string>

#include "core/base/byte_buffer_dynamic.h"
#include "core/base/noncopyable.h"

namespace squirrel {

namespace test {

class HttpResponse {
 public:
 enum Version { UNKNOWN = 0, HTTP10, HTTP11, };
             
  enum HttpStatusCode {
    STATUS_UNKNOWN = 0,
    STATUS_200_OK = 200,
    STATUS_301_MOVE_PERMANENTLY = 301,
    STATUS_400_BAD_REQUEST = 400,
    STATUS_404_NOT_FOUND = 404,
  };

  enum HttpResponseParseState {
    EXPECT_RESPONSE_LINE = 0,
    EXPECT_HEADER,
    EXPECT_BODY,
    EXPECT_COMPLETE,
  };

  HttpResponse() : status_(STATUS_UNKNOWN),
    state_(EXPECT_RESPONSE_LINE), body_length_(0), version_(UNKNOWN) {}

  ~HttpResponse() {}

  inline void Reset() {
    this->header_.clear();
    this->status_ = STATUS_UNKNOWN;
    this->status_message_.clear();
    this->body_.clear();
    this->status_message_.clear();
    this->state_ = EXPECT_RESPONSE_LINE;
    this->body_length_ = 0;
    this->version_ = UNKNOWN;
  }

  inline void SetVersion(Version version) { this->version_ = version; }
  inline Version GetVersion() const { return this->version_; }

  inline bool CheckExpectResponseLine() const {
    return this->state_ == EXPECT_RESPONSE_LINE;
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

  inline void ReceiveResponseLine() {
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

  inline void SetStatusCode(HttpStatusCode status) {
    this->status_ = status;
  }
  inline void SetStatusMessage(const std::string &message) {
    this->status_message_ = message;
  }
  void AddHeader(const char *start, const char *colon, const char *end) {
    std::string field(start, colon);
    ++colon;
    while (isspace(*colon)) {
      ++colon;
    }     
    std::string value(colon, end);
    while (!value.empty() && isspace(value[value.size()-1])) {
      value.resize(value.size()-1);
    } 
    header_[field] = value;
    if(field == "Content-Length") {
      this->body_length_ = atoi(value.c_str());
    } 
  }
  inline int GetBodyLength() const { return this->body_length_; }
  inline void SetBody(const std::string &body) {
    this->body_ = body;
  }
  inline void SetBody(const char *buffer, size_t size) {
    this->body_.assign(buffer, size);
  }
  inline const char *GetBody() { return this->body_.data(); }
  inline size_t GetBodySize() { return this->body_.size(); }

 private:
  inline void AddHeader(const std::string &key, const std::string &value) {
    this->header_[key] = value;
  }

  std::map<std::string , std::string> header_;
  HttpStatusCode status_;
  std::string status_message_;
  std::string body_;

  HttpResponseParseState state_;
  int body_length_;
  Version version_;
};

}  // namespace test

}  // namespace squirrel

#endif  // _squirrel_test__HTTP__RESPONSE__H

