//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-14 14:28:42.
// File name: http_client.cc
//
// Description: 
// Define class HttpClient.
//

#include "test/http_client.h"

#include <sstream>
#include <boost/bind.hpp>

#include "global/async_logging_singleton.h"

namespace squirrel {

using namespace global;

namespace test {

HttpClient::HttpClient(const core::InetAddress &server_address,
    const std::string &name) : client_(server_address, name), buffer_(sizeof(core::uint64)) {}

HttpClient::~HttpClient() {}

bool HttpClient::Initialize(core::EventLoop *loop) {
  assert(loop);

  if(this->client_.Initialize(loop) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) Initialize TCPClient failed",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  this->client_.SetConnectionCallback(boost::bind(&HttpClient::OnConnection, this, _1));
  this->client_.SetMessageCallback(boost::bind(&HttpClient::OnMessage, this, _1, _2, _3));
  this->client_.SetWriteCallback(boost::bind(&HttpClient::OnWriteComplete, this, _1));

  return true;
}

void HttpClient::Start() {
  this->client_.Start();
}

void HttpClient::Stop() {
  this->client_.Start();
}

void HttpClient::Get(const char *message, size_t size) {
}

void HttpClient::Post(const char *message, size_t size) {
  if(this->connection_) {
    std::ostringstream oss;
    oss << kHttpPostStr << kHttpPostAcceptStr << kHttpPostConnectionStr
      << kHttpPostUserAgentStr << kHttpPostContentLength << size << kCRLF
      << kHttpPostContentType;
  /*
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
        "%s:%d (%s) Post head begin ----------------------\n%s",
        __FILE__, __LINE__, __FUNCTION__, oss.str().c_str());
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
        "%s:%d (%s) Post head end ----------------------",
        __FILE__, __LINE__, __FUNCTION__);
  */
    this->connection_->SendMessage(oss.str().c_str(), oss.str().length());
    this->connection_->SendMessage(message, size);
  }
}

void HttpClient::OnConnection(const core::TCPConnectionPtr &connection) {
  if(connection->CheckConnected()) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "Http connection established.");
    connection->GetContext() = HttpResponse();
    this->connection_ = connection;
    this->OnConnected();
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "Http connection disconnected.");
    this->OnDisconnected();
    this->connection_.reset();
  }
}

void HttpClient::OnMessage(const core::TCPConnectionPtr &connection,
    core::ByteBufferDynamic &buffer, const core::TimeTick &time_tick) {
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
      "[HttpClient] OnMessage.");
  HttpResponse *response = &boost::any_cast<HttpResponse &>(connection->GetContext());
  assert(response);
  while(true) {
    char buff[32 * 1024];
    memcpy(buff, buffer.ReadPointer(), buffer.ReadSize());
    buff[buffer.ReadSize()] = '\0';
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
        "[CheckExpectAll] [%s].", buff);
    if(this->ParseHttpResponse(buffer, response) == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "HttpClient::OnMessage Need to shutdown.");
      connection->SendMessage(kBadRequest);
      connection->Shutdown();
      break;
    } else {
      if(response->CheckExpectAll()) {
        if(this->OnMessage(response->GetBody(), response->GetBodySize()) == false) {
          connection->SendMessage(kBadRequest);
          connection->Shutdown();
        }
      }
      break;
    }
  }
}

void HttpClient::OnWriteComplete(const core::TCPConnectionPtr &connection) {
  HttpResponse *response = &boost::any_cast<HttpResponse &>(connection->GetContext());
  response->Reset();
}

bool HttpClient::ParseHttpResponse(core::ByteBufferDynamic &buffer, HttpResponse *response) {
  if(response->CheckExpectResponseLine()) {
    if(buffer.ReadSize() >= 2) {
      const char* crlf = std::search(buffer.ReadPointer(),
          static_cast<const char*>(buffer.WritePointer()), kCRLF, kCRLF + 2);
      if(crlf < buffer.WritePointer()) {
        const char* start = buffer.ReadPointer();
        const char* space = std::find(start, crlf, ' ');
        if(space != crlf) {
          if(space - start == 8 && std::equal(start, space-1, "HTTP/1.")) {
            if(*(space-1) == '1') {
              response->SetVersion(HttpResponse::HTTP11);
            } else if (*(space-1) == '0') {
              response->SetVersion(HttpResponse::HTTP10);
            } else {
              return false;
            }
            response->SetStatusMessage(std::string(space+1, crlf - space - 1));
            response->ReceiveResponseLine();
            assert(buffer.ReadPointer() <= crlf + 2);
            buffer.ReadShift(crlf + 2 - buffer.ReadPointer());
          } else {
            return false;
          }
        } else {
          return true;
        }
      } else {
        return true;
      }
    } else {
      return true;
    }
  }
  if(response->CheckExpectHeader()) {
    while(true) {
      if(buffer.ReadSize() >= 2) {
        const char* crlf = std::search(buffer.ReadPointer(),
            static_cast<const char*>(buffer.WritePointer()), kCRLF, kCRLF + 2);
        if(crlf < buffer.WritePointer()) {
          const char* colon = std::find(buffer.ReadPointer(), crlf, ':');
          if(colon != crlf) {
            response->AddHeader(buffer.ReadPointer(), colon, crlf);
            assert(buffer.ReadPointer() <= crlf + 2);
            buffer.ReadShift(crlf + 2 - buffer.ReadPointer());
            continue;
          } else {
            response->ReceiveHeader();
            assert(buffer.ReadPointer() <= crlf + 2);
            buffer.ReadShift(crlf + 2 - buffer.ReadPointer());
            break;
          }
        } else {
          return true;
        }
      } else {
        return true;
      }
    }
  }
  if(response->CheckExpectBody()) {
    int body_length = response->GetBodyLength();
    if(body_length > 0) {
      if(body_length <= buffer.ReadSize()) {
        response->SetBody(buffer.ReadPointer(), body_length);
        buffer.ReadShift(body_length);
        response->ReceiveBody();
      }
    }
    return true;
  }
  return true;
}

}  // namespace squirrel

}  // namespace test

