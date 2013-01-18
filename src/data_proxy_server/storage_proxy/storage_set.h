//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-07 15:15:57.
// File name: storage_set.h
//
// Description: 
// Define class StorageSet.
//

#ifndef _squirrel_dataproxy_storage__STORAGE__SET__H
#define _squirrel_dataproxy_storage__STORAGE__SET__H

#include "core/base/noncopyable.h"
#include "data_proxy_server/storage_proxy/procedure_configure.h"
#include "data_proxy_server/storage_proxy/storage.h"
#include "data_proxy_server/storage_proxy/storage_configure.h"
#include "global/elf_hash.h"

namespace squirrel {

namespace dataproxy {

namespace storage {

class StorageSet : public core::Noncopyable {
  typedef std::vector<StoragePtr> StorageVec;
 public:
  StorageSet();
  ~StorageSet();

  bool Initialize(const StorageConfigure &configure,
      const ProcedureConfigure &procedure);
  bool Start();

  inline StoragePtr GetStorage(core::uint32 hash_value) {
    if(this->storages_.empty()) {
      return StoragePtr();
    } else {
      return this->storages_[hash_value % this->storages_.size()];
    }
  }

  inline StoragePtr GetStorage(const std::string &key) {
    return this->GetStorage(global::elf_hash(key.data(), key.size()));
  }

 private:
  StorageVec storages_;
  // TODO: connection thread, when any connection disconnected,
  // restart to connect mysql server.
};

}  // namespace storage

}  // namespace dataproxy

}  // namespace squirrel

#endif  // _squirrel_dataproxy_storage__STORAGE__SET__H

