//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-05 18:26:40.
// File name: redis_set.h
//
// Description: 
// Define class RedisSet.
//

#ifndef _squirrel_dataproxy_redis__REDIS__SET__H
#define _squirrel_dataproxy_redis__REDIS__SET__H

#include "core/base/noncopyable.h"
#include "data_proxy_server/redis_proxy/redis.h"
#include "data_proxy_server/redis_proxy/redis_configure.h"
#include "global/elf_hash.h"

namespace squirrel {

namespace dataproxy {

namespace redis {

class RedisSet : public core::Noncopyable {
  typedef std::vector<RedisPtr> RedisVec;
 public:
  RedisSet();
  ~RedisSet();

  bool Initialize(const RedisConfigure &configure);
  bool Start();

  inline RedisPtr GetRedis(core::uint32 hash_value) {
    if(this->redises_.empty()) {
      return RedisPtr();
    } else {
      return this->redises_[hash_value % this->redises_.size()];
    }
  }

  inline RedisPtr GetRedis(const std::string &key) {
    return this->GetRedis(global::elf_hash(key.data(), key.size()));
  }

 private:
  RedisVec redises_;
  // TODO: connection thread, when any connection disconnected,
  // restart to connect redis server.
};

}  // redis

}  // namespace dataproxy

}  // namespace squirrel

#endif  // _squirrel_dataproxy_redis__REDIS__SET__H

