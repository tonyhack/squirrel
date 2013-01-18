//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-07 15:30:16.
// File name: storage_set.cc
//
// Description: 
//
//

#include "data_proxy_server/storage_proxy/storage_set.h"

#include "data_proxy_server/storage_proxy/storage.h"
#include "data_proxy_server/storage_proxy/storage_configure.h"
#include "global/async_logging_singleton.h"

namespace squirrel {

using namespace global;

namespace dataproxy {

namespace storage {

StorageSet::StorageSet() {}

StorageSet::~StorageSet() {}

bool StorageSet::Initialize(const StorageConfigure &configure,
    const ProcedureConfigure &procedure) {
  const StorageConfigure::ConnectionConfigureVec &connections =
    configure.GetConnectionConfigure();
  StorageConfigure::ConnectionConfigureVec::const_iterator iterator = 
    connections.begin();
  for(; iterator != connections.end(); ++iterator) {
    Storage *storage = new (std::nothrow) Storage();
    if(storage == NULL) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [StorageSet] Allocate Storage failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    if(storage->Initialize(iterator->host_, iterator->port_, iterator->user_,
          iterator->passwd_, iterator->db_, procedure) == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [StorageSet] Initialize Storage failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    this->storages_.push_back(StoragePtr(storage));
  }

  return true;
}

bool StorageSet::Start() {
  StorageVec::iterator iterator = this->storages_.begin();
  for(; iterator != this->storages_.end(); ++iterator) {
    if((*iterator)->Start() == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [StorageSet] Start Storage failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
  }
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "%s:%d (%s) [StorageSet] Start Storage complete.",
      __FILE__, __LINE__, __FUNCTION__);

  return true;
}

}  // namespace storage

}  // namespace dataproxy

}  // namespace squirrel

