//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-07 15:23:17.
// File name: storage_configure.h
//
// Description: 
// Define class StorageConfigure.
//

#ifndef _squirrel_dataproxy_storage__STORAGE__CONFIGURE__H
#define _squirrel_dataproxy_storage__STORAGE__CONFIGURE__H

#include "core/base/noncopyable.h"

namespace squirrel {

namespace dataproxy {

namespace storage {

struct Connections {
  std::string db_;
  std::string host_;
  std::string user_;
  std::string passwd_;
  core::uint16 port_;
};

class StorageConfigure : public core::Noncopyable {
 public:
  typedef std::vector<Connections> ConnectionConfigureVec;

  StorageConfigure() {}
  ~StorageConfigure() {}

  inline void AddConnection(const Connections &configure) {
    this->connections_.push_back(configure);
  }

  inline const ConnectionConfigureVec &GetConnectionConfigure() const {
    return this->connections_;
  }

 private:
  ConnectionConfigureVec connections_;
};

}  // storage

}  // namespace dataproxy

}  // namespace squirrel

#endif  // _squirrel_dataproxy_storage__STORAGE__CONFIGURE__H

