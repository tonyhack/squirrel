//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-04 10:16:57.
// File name: request_terminal_set.h
//
// Description: 
// Define class RequestTerminalSet.
//

#ifndef _squirrel_dataproxy_server__REQUEST__TERMINAL__SET__H
#define _squirrel_dataproxy_server__REQUEST__TERMINAL__SET__H

#include "core/base/mutex.h"
#include "core/base/noncopyable.h"
// #include "global/thrift_packet.h"
#include "data_proxy_server/server/request_terminal.h"
#include "global/sequence_allocator.h"

namespace squirrel {

namespace dataproxy {

namespace server {

class RequestTerminalSet : public core::Noncopyable {
  typedef std::map<core::uint64, RequestTerminalPtr> TerminalMap;
 public:
  RequestTerminalSet();
  ~RequestTerminalSet();

  // Login/Logout server.
  bool LoginDataproxyServer(const RequestTerminalPtr &terminal);
  bool LogoutDataproxyServer(const RequestTerminalPtr &terminal);

  RequestTerminalPtr GetRequestTerminal(core::uint64 guid);

 private:
  core::Mutex mutex_;
  TerminalMap terminals_;

  global::MTSequenceAllocator sequence_;
  // global::ThriftPacket packet_;
};

class RequestTerminalSetSingleton : public core::Noncopyable {
 public:
  RequestTerminalSetSingleton () {}
  ~RequestTerminalSetSingleton () {}

  static inline RequestTerminalSet &GetRequestTerminalSet() {
    return request_terminal_set_;
  }

 private:
  static RequestTerminalSet request_terminal_set_;
};

}  // namespace server

}  // namespace dataproxy

}  // namespace squirrel

#endif  // _squirrel_dataproxy_server__REQUEST__TERMINAL__SET__H

