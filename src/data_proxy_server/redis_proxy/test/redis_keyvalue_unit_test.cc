//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-12 14:42:03.
// File name: redis_keyvalue_unit_test.cc
//
// Description: 
// Unit test of redis key-value request.
//

#include "common/keyvalue_message_types.h"
#include "data_proxy_server/redis_proxy/redis.h"
#include "global/async_logging_singleton.h"

using namespace squirrel;
using namespace squirrel::dataproxy;
using namespace squirrel::dataproxy::redis;

bool TestSet(Redis &redis, const std::string &key, const std::string &value) {
  int retcode = -1;
  retcode = redis.KeyvalueSet(key, value);
  if(retcode != common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] KeyvalueSet [%s, %s] error.\n", key.c_str(), value.c_str());
    return false;
  }
  printf("Set key[%s] value[%s] ok\n", key.c_str(), value.c_str());
  return true;
}

bool TestGet(Redis &redis, const std::string &key, const std::string &value) {
  int retcode = -1;
  std::string val;
  retcode = redis.KeyvalueGet(key, val);
  if(retcode != common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] KeyvalueGet [%s] error.\n", key.c_str());
    return false;
  } else {
    printf("Get key[%s] value[%s] ok\n", key.c_str(), val.c_str());
    return value == val;
  }
}

bool TestDelete(Redis &redis, const std::string &key) {
  int retcode = -1;
  retcode = redis.KeyvalueDelete(key);
  if(retcode != common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] KeyvalueDelete [%s] error.\n", key.c_str());
    return false;
  } else {
    printf("Delete key[%s] ok\n", key.c_str());
    return true;
  }
}

int main(int argc, char *argv[]) {
  if(argc != 3) {
    printf("Bad argument.\n");
    return 0;
  }

  int port = atoi(argv[2]);

  if(global::AsyncLoggingSingleton::GetAsyncLogging()->Initialize(
        std::string(argv[0]) + ".log", 64 * 1024) == false) {
    printf("Initialize logging failed.\n");
    return 0;
  }

  global::AsyncLoggingSingleton::GetAsyncLogging()->Start();

  Redis redis;

  if(redis.Initialize(argv[1], port) == false) {
    printf("[Failure] Redis initialize failed.\n");
    return 0;
  }

  if(redis.Start() == false) {
    printf("[Failure] Redis start failed.\n");
    return 0;
  }

  if(redis.CheckConnectState() == false) {
    printf("[Failure] CheckConnectState false.\n");
    return 0;
  }

  int total = 0;

  int ok = 0;
  int failure = 0;

  std::string key, value;



  key = "KEYVALUE-TEST-001";
  value = "Tony001";
  ++total;
  TestSet(redis, key, value) ? ++ok : ++failure;

  key = "KEYVALUE-TEST-002";
  value = "Tony002";
  ++total;
  TestSet(redis, key, value) ? ++ok : ++failure;

  key = "KEYVALUE-TEST-003";
  value = "Tony003";
  ++total;
  TestSet(redis, key, value) ? ++ok : ++failure;

  key = "KEYVALUE-TEST-004";
  value = "Tony004";
  ++total;
  TestSet(redis, key, value) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  key = "KEYVALUE-TEST-001";
  value = "Tony001";
  ++total;
  TestGet(redis, key, value) ? ++ok : ++failure;

  key = "KEYVALUE-TEST-002";
  value = "Tony002";
  ++total;
  TestGet(redis, key, value) ? ++ok : ++failure;

  key = "KEYVALUE-TEST-003";
  value = "Tony003";
  ++total;
  TestGet(redis, key, value) ? ++ok : ++failure;

  key = "KEYVALUE-TEST-004";
  value = "Tony004";
  ++total;
  TestGet(redis, key, value) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  key = "KEYVALUE-TEST-001";
  ++total;
  TestDelete(redis, key) ? ++ok : ++failure;

  key = "KEYVALUE-TEST-002";
  ++total;
  TestDelete(redis, key) ? ++ok : ++failure;

  key = "KEYVALUE-TEST-003";
  ++total;
  TestDelete(redis, key) ? ++ok : ++failure;

  key = "KEYVALUE-TEST-004";
  ++total;
  TestDelete(redis, key) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  key = "KEYVALUE-TEST-001";
  value = "Tony001";
  ++total;
  TestGet(redis, key, value) ? ++failure : ++ok;
                                               
  key = "KEYVALUE-TEST-002";                   
  value = "Tony002";                           
  ++total;                                     
  TestGet(redis, key, value) ? ++failure : ++ok;
                                               
  key = "KEYVALUE-TEST-003";                   
  value = "Tony003";                           
  ++total;                                     
  TestGet(redis, key, value) ? ++failure : ++ok;
                                               
  key = "KEYVALUE-TEST-004";                   
  value = "Tony004";                           
  ++total;                                     
  TestGet(redis, key, value) ? ++failure : ++ok;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  global::AsyncLoggingSingleton::GetAsyncLogging()->Stop();

  return 0;
}

