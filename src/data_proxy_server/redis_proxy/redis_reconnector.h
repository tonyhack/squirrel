//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-08 21:03:46.
// File name: redis_reconnector.h
//
// Description: 
// Define class RedisReconnector.
//

#ifndef _squirrel_dataproxy_redis__REDIS__RECONNECTOR__H
#define _squirrel_dataproxy_redis__REDIS__RECONNECTOR__H

#include "data_proxy_server/redis_proxy/redis.h"
#include "global/reconnector.h"

namespace squirrel {

namespace dataproxy {

namespace redis {

class RedisReconnectRequest {
 public:
  RedisReconnectRequest() : valid_(true), reconnect_(true) {}
  RedisReconnectRequest(const RedisPtr &redis)
    : valid_(true), reconnect_(true), redis_(redis) {}
  ~RedisReconnectRequest() {}

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
    if(redis_) {
      return redis_->Reconnect();
    }
    return true;
  }

 private:
  bool valid_;
  bool reconnect_;
  RedisPtr redis_;
};

typedef global::Reconnector<RedisReconnectRequest> RedisReconnector;

}  // namespace redis

}  // namespace dataproxy

}  // namespace squirrel

#endif  // _squirrel_dataproxy_redis__REDIS__RECONNECTOR__H

