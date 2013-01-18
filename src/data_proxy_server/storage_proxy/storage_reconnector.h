//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-08 21:56:54.
// File name: storage_reconnector.h
//
// Description: 
// Define class StorageReconnector.
//

#ifndef _squirrel_dataproxy_storage__STORAGE__RECONNECTOR__H
#define _squirrel_dataproxy_storage__STORAGE__RECONNECTOR__H

#include "data_proxy_server/storage_proxy/storage.h"
#include "global/reconnector.h"

namespace squirrel {

namespace dataproxy {

namespace storage {

class StorageReconnectRequest {
 public:
  StorageReconnectRequest() : valid_(true), reconnect_(true) {}
  StorageReconnectRequest(const StoragePtr &storage)
    : valid_(true), reconnect_(true), storage_(storage) {}
  ~StorageReconnectRequest() {}

  inline bool CheckValid() const {
    return this->valid_;
  }

  inline bool CheckReconnect() const {
    return this->reconnect_;
  }

  inline void SetValid(bool valid) {
    this->valid_ = valid;
  }

  inline void SetReconnect(bool reconnect) {
    this->reconnect_ = reconnect;
  }

  inline bool Reconnect() const {
    if(storage_) {
      return storage_->Reconnect();
    }
    return true;
  }

 private:
  bool valid_;
  bool reconnect_;
  StoragePtr storage_;
};

typedef global::Reconnector<StorageReconnectRequest> StorageReconnector;

}  // namespace storage

}  // namespace dataproxy

}  // namespace squirrel

#endif  // _squirrel_dataproxy_storage__STORAGE__RECONNECTOR__H

