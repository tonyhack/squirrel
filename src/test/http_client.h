//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-14 14:19:46.
// File name: http_client.h
//
// Description: 
// Define class HttpClient.
//

#ifndef _squirrel_test__HTTP__CLIENT__H
#define _squirrel_test__HTTP__CLIENT__H

#include "core/tcp_client.h"
#include "core/base/noncopyable.h"
#include "test/http_response.h"

namespace squirrel {

namespace test {

static const char *kHttpPostStr = "POST / HTTP/1.1\r\n";
static const char *kHttpPostAcceptStr = "Accept: thrift-0.8\r\n";
static const char *kHttpPostConnectionStr = "Connection: Keep-Alive\r\n";
static const char *kHttpPostUserAgentStr = "User-Agent: Squirrel test client\r\n";
static const char *kHttpPostContentLength = "Content-Length: ";
static const char *kHttpPostContentType = "Content-Type: thrift-0.8\r\n\r\n";
static const char *kBadRequest = "HTTP/1.1 400 Bad Request\r\n\r\n";
static const char *kCRLF = "\r\n";

class HttpClient : public core::Noncopyable {
 public:
  HttpClient(const core::InetAddress &server_address,
      const std::string &name);
  virtual ~HttpClient();

  bool Initialize(core::EventLoop *loop);

  void Start();
  void Stop();

  void Get(const char *message, size_t size);
  void Post(const char *message, size_t size);

  virtual bool OnMessage(const char *message, size_t size) = 0;
  virtual void OnConnected() = 0;
  virtual void OnDisconnected() = 0;

 private:
  void OnConnection(const core::TCPConnectionPtr &connection);
  void OnMessage(const core::TCPConnectionPtr &connection,
      core::ByteBufferDynamic &buffer, const core::TimeTick &time_tick);
  void OnWriteComplete(const core::TCPConnectionPtr &connection);

  bool ParseHttpResponse(core::ByteBufferDynamic &buffer, HttpResponse *response);

  core::TCPClient client_;
  core::TCPConnectionPtr connection_;
  core::ByteBufferDynamic buffer_;
};

}  // namespace test

}  // namespace squirrel

#endif  // _squirrel_test__HTTP__CLIENT__H

