//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-05 18:48:54.
// File name: redis_configure.h
//
// Description: 
// Define class RedisConfigure.
//

#ifndef _squirrel_dataproxy_redis__REDIS__CONFIGURE__H
#define _squirrel_dataproxy_redis__REDIS__CONFIGURE__H

#include <vector>

#include "core/base/noncopyable.h"

namespace squirrel {

namespace dataproxy {

namespace redis {

class RedisConfigure : public core::Noncopyable {
 public:
  typedef std::pair<std::string, core::uint16> Connections;
  typedef std::vector<Connections> ConnectionConfigureVec;

  RedisConfigure() {}
  ~RedisConfigure() {}

  inline void AddConnection(const std::string &host, core::uint16 port) {
    this->connections_.push_back(std::make_pair(host, port));
  }

  inline const ConnectionConfigureVec &GetConnectionConfigure() const {
    return this->connections_;
  }

 private:
  ConnectionConfigureVec connections_;
};

}  // redis

}  // namespace dataproxy

}  // namespace squirrel

#endif  // _squirrel_dataproxy_redis__REDIS__CONFIGURE__H

