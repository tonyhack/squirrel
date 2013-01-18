//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-05 18:56:43.
// File name: redis_set.cc
//
// Description: 
// Define class RedisSet.
//

#include "data_proxy_server/redis_proxy/redis_set.h"

#include "data_proxy_server/redis_proxy/redis.h"
#include "data_proxy_server/redis_proxy/redis_configure.h"
#include "global/async_logging_singleton.h"

namespace squirrel {

using namespace global;

namespace dataproxy {

namespace redis {

RedisSet::RedisSet() {}
RedisSet::~RedisSet() {}

bool RedisSet::Initialize(const RedisConfigure &configure) {
  const RedisConfigure::ConnectionConfigureVec &connections =
    configure.GetConnectionConfigure();
  RedisConfigure::ConnectionConfigureVec::const_iterator iterator = 
    connections.begin();
  for(; iterator != connections.end(); ++iterator) {
    Redis *redis = new (std::nothrow) Redis();
    if(redis == NULL) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [RedisSet] Allocate Redis failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    if(redis->Initialize(iterator->first, iterator->second) == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [RedisSet] Initialize Redis failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    this->redises_.push_back(RedisPtr(redis));
  }
  
  return true;
}

bool RedisSet::Start() {
  RedisVec::iterator iterator = this->redises_.begin();
  for(; iterator != this->redises_.end(); ++iterator) {
    if((*iterator)->Start() == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [RedisSet] Start Redis failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
  }
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
      "%s:%d (%s) [RedisSet] Start Redis complete.",
      __FILE__, __LINE__, __FUNCTION__);

  return true;
}

}  // redis

}  // namespace dataproxy

}  // namespace squirrel

