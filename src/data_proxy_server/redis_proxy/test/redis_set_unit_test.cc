//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-12 15:57:11.
// File name: redis_set_unit_test.cc
//
// Description: 
// Unit test of redis set.
//

#include "common/keyvalue_message_types.h"
#include "data_proxy_server/redis_proxy/redis_set.h"
#include "global/async_logging_singleton.h"

using namespace squirrel;
using namespace squirrel::dataproxy;
using namespace squirrel::dataproxy::redis;

int total = 0;
int ok = 0;
int failure = 0;

bool TestSet(RedisSet &set, const std::string &key, const std::string &value) {
  int retcode = -1;
  RedisPtr redis = set.GetRedis(key);
  if(redis == NULL) {
    printf("[Failure] GetRedis failed on Set [%s, %s].\n", key.c_str(), value.c_str());
    ++failure;
    return false;
  }
  printf("redis is [%p]\n", redis.get());
  retcode = redis->KeyvalueSet(key, value);
  if(retcode != common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] KeyvalueSet [%s, %s] error.\n", key.c_str(), value.c_str());
    ++failure;
    return false;
  }
  printf("Set key[%s] value[%s] ok\n", key.c_str(), value.c_str());
  ++ok;
  return true;
}

bool TestGet(RedisSet &set, const std::string &key, const std::string &value) {
  int retcode = -1;
  std::string val;
  RedisPtr redis = set.GetRedis(key);
  if(redis == NULL) {
    printf("[Failure] GetRedis failed on Get [%s, %s].\n", key.c_str(), value.c_str());
    ++failure;
    return false;
  }
  printf("redis is [%p]\n", redis.get());
  retcode = redis->KeyvalueGet(key, val);
  if(retcode != common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] KeyvalueGet [%s] error.\n", key.c_str());
    ++failure;
    return false;
  } else {
    printf("Get key[%s] value[%s] ok\n", key.c_str(), val.c_str());
    if(value == val) {
      ++ok;
      return true;
    }
    ++failure;
    return false;
  }
}

bool TestDelete(RedisSet &set, const std::string &key) {
  int retcode = -1;
  RedisPtr redis = set.GetRedis(key);
  if(redis == NULL) {
    printf("[Failure] GetRedis failed on Delete [%s].\n", key.c_str());
    ++failure;
    return false;
  }
  printf("redis is [%p]\n", redis.get());
  retcode = redis->KeyvalueDelete(key);
  if(retcode != common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] KeyvalueDelete [%s] error.\n", key.c_str());
    ++failure;
    return false;
  } else {
    printf("Delete key[%s] ok\n", key.c_str());
    ++ok;
    return true;
  }
}

int main(int argc, char *argv[]) {
  RedisConfigure configure;

  std::string host;
  int port;
  for(int i = 1; i < argc - 1; i += 2) {
    host = argv[i];
    port = atoi(argv[i + 1]);
    printf("server info[%s:%d]\n", host.c_str(), port);
    configure.AddConnection(host, port);
  }

  const RedisConfigure::ConnectionConfigureVec &vec = configure.GetConnectionConfigure();
  if(vec.empty()) {
    printf("Bad argument.\n");
    return 0;
  }

  if(global::AsyncLoggingSingleton::GetAsyncLogging()->Initialize(
        std::string(argv[0]) + ".log", 64 * 1024) == false) {
    printf("Initialize logging failed.\n");
    return 0;
  }

  global::AsyncLoggingSingleton::GetAsyncLogging()->Start();

  RedisSet set;

  if(set.Initialize(configure) == false) {
    printf("[Failure] Redis set initialize failed.\n");
    return 0;
  }
  if(set.Start() == false) {
    printf("[Failure] Redis set start failed.\n");
    return 0;
  }


  std::string key, value;



  key = "KEYVALUE-TEST-001";
  value = "Tony001";
  ++total;
  TestSet(set, key, value);

  key = "KEYVALUE-TEST-002";
  value = "Tony002";
  ++total;
  TestSet(set, key, value);

  key = "KEYVALUE-TEST-003";
  value = "Tony003";
  ++total;
  TestSet(set, key, value);

  key = "KEYVALUE-TEST-004";
  value = "Tony004";
  ++total;
  TestSet(set, key, value);

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  key = "KEYVALUE-TEST-001";
  value = "Tony001";
  ++total;
  TestGet(set, key, value);

  key = "KEYVALUE-TEST-002";
  value = "Tony002";
  ++total;
  TestGet(set, key, value);

  key = "KEYVALUE-TEST-003";
  value = "Tony003";
  ++total;
  TestGet(set, key, value);

  key = "KEYVALUE-TEST-004";
  value = "Tony004";
  ++total;
  TestGet(set, key, value);

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  key = "KEYVALUE-TEST-001";
  ++total;
  TestDelete(set, key);

  key = "KEYVALUE-TEST-002";
  ++total;
  TestDelete(set, key);

  key = "KEYVALUE-TEST-003";
  ++total;
  TestDelete(set, key);

  key = "KEYVALUE-TEST-004";
  ++total;
  TestDelete(set, key);

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  key = "KEYVALUE-TEST-001";
  value = "Tony001";
  ++total;
  TestGet(set, key, value);
                                               
  key = "KEYVALUE-TEST-002";                   
  value = "Tony002";                           
  ++total;                                     
  TestGet(set, key, value);
                                               
  key = "KEYVALUE-TEST-003";                   
  value = "Tony003";                           
  ++total;                                     
  TestGet(set, key, value);
                                               
  key = "KEYVALUE-TEST-004";                   
  value = "Tony004";                           
  ++total;                                     
  TestGet(set, key, value);

  printf("[Result   ] total=%d ok=%d failure=%d\n", total, ok, failure);
  printf("[Reference] total=16 ok=12 failure=4\n");




  global::AsyncLoggingSingleton::GetAsyncLogging()->Stop();

  return 0;
}

