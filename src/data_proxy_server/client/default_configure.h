//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-10 14:43:46.
// File name: default_configure.h
//
// Description: 
// Define class DefaultConfigure.
//

#ifndef _squirrel_dataproxy_client__DEFAULT__CONFIGURE__H
#define _squirrel_dataproxy_client__DEFAULT__CONFIGURE__H

#include "common/server_type_types.h"
#include "core/base/noncopyable.h"

namespace squirrel {

namespace dataproxy {

namespace client {

class DefaultConfigure : public core::Noncopyable {
 public:
  typedef std::vector<common::ServerSignature> ServerSignatureVec;

  DefaultConfigure() {}
  ~DefaultConfigure() {}

  inline void Insert(const common::ServerSignature &server) {
    this->server_signatures_.push_back(server);
  }

  inline bool HashGet(size_t hash, common::ServerSignature &server) {
    size_t size = this->server_signatures_.size();
    if(size > 0) {
      server = this->server_signatures_[hash % size];
      return true;
    } else {
      return false;
    }
  }

  inline const ServerSignatureVec &GetServerSignature() const {
    return this->server_signatures_;
  }

 private:
  ServerSignatureVec server_signatures_;
};

class DefaultConfigureSingleton : public core::Noncopyable {
 public:
  DefaultConfigureSingleton() {}
  ~DefaultConfigureSingleton() {}

  static inline DefaultConfigure &GetDefaultConfigure() {
    return default_configure_;
  }

 private:
  static DefaultConfigure default_configure_;
};

}  // namespace client

}  // namespace dataproxy

}  // namespace squirrel

#endif  // _squirrel_dataproxy_client__DEFAULT__CONFIGURE__H

