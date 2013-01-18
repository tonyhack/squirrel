//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-19 19:53:45.
// File name: redis_number_unit_test.cc
//
// Description: 
// Unit test of redis number request.
//

#include "common/number_message_types.h"
#include "data_proxy_server/redis_proxy/redis.h"
#include "global/async_logging_singleton.h"

using namespace squirrel;
using namespace squirrel::dataproxy;
using namespace squirrel::dataproxy::redis;

bool TestIncrease(Redis &redis, const std::string &key, const std::string &increment) {
  int retcode = -1;
  std::string val;
  retcode = redis.NumberIncrease(key, increment, val);
  if(retcode != common::NumberResponseType::NUMBER_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] NumberIncrease [%s, %s]\n", key.c_str(), increment.c_str());
    return false;
  }
  printf("Increase key[%s] increment[%s] value[%s] ok\n", key.c_str(), increment.c_str(), val.c_str());
  return true;
}

bool TestDecrease(Redis &redis, const std::string &key, const std::string &decrement) {
  int retcode = -1;
  std::string val;
  retcode = redis.NumberDecrease(key, decrement, val);
  if(retcode != common::NumberResponseType::NUMBER_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] NumberDecrease [%s, %s]\n", key.c_str(), decrement.c_str());
    return false;
  }
  printf("Decrease key[%s] decrement[%s] value[%s] ok\n", key.c_str(), decrement.c_str(), val.c_str());
  return true;
}

bool TestReset(Redis &redis, const std::string &key) {
  int retcode = -1;
  std::string val;
  retcode = redis.NumberReset(key);
  if(retcode != common::NumberResponseType::NUMBER_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] NumberReset [%s]\n", key.c_str());
    return false;
  }
  printf("Reset key[%s] ok\n", key.c_str());
  return true;
}

bool TestDelete(Redis &redis, const std::string &key) {
  int retcode = -1;
  std::string val;
  retcode = redis.NumberDelete(key);
  if(retcode != common::NumberResponseType::NUMBER_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] NumberDelete [%s]\n", key.c_str());
    return false;
  }
  printf("Delete key[%s] ok\n", key.c_str());
  return true;
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

  std::string key, xcrease;





  key = "NUMBER-TEST-001";
  xcrease = "10";
  ++total;
  TestIncrease(redis, key, xcrease) ? ++ok : ++failure;

  key = "NUMBER-TEST-002";
  xcrease = "20";
  ++total;
  TestIncrease(redis, key, xcrease) ? ++ok : ++failure;

  key = "NUMBER-TEST-003";
  xcrease = "30";
  ++total;
  TestIncrease(redis, key, xcrease) ? ++ok : ++failure;

  key = "NUMBER-TEST-004";
  xcrease = "40";
  ++total;
  TestIncrease(redis, key, xcrease) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);




  key = "NUMBER-TEST-001";
  xcrease = "5";
  ++total;
  TestDecrease(redis, key, xcrease) ? ++ok : ++failure;

  key = "NUMBER-TEST-002";
  xcrease = "10";
  ++total;
  TestDecrease(redis, key, xcrease) ? ++ok : ++failure;

  key = "NUMBER-TEST-003";
  xcrease = "15";
  ++total;
  TestDecrease(redis, key, xcrease) ? ++ok : ++failure;

  key = "NUMBER-TEST-004";
  xcrease = "20";
  ++total;
  TestDecrease(redis, key, xcrease) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);




  key = "NUMBER-TEST-001";
  ++total;
  TestReset(redis, key) ? ++ok : ++failure;

  key = "NUMBER-TEST-002";
  ++total;
  TestReset(redis, key) ? ++ok : ++failure;

  key = "NUMBER-TEST-003";
  ++total;
  TestReset(redis, key) ? ++ok : ++failure;

  key = "NUMBER-TEST-004";
  ++total;
  TestReset(redis, key) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);




  key = "NUMBER-TEST-001";
  ++total;
  TestDelete(redis, key) ? ++ok : ++failure;

  key = "NUMBER-TEST-002";
  ++total;
  TestDelete(redis, key) ? ++ok : ++failure;

  key = "NUMBER-TEST-003";
  ++total;
  TestDelete(redis, key) ? ++ok : ++failure;

  key = "NUMBER-TEST-004";
  ++total;
  TestDelete(redis, key) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);




  key = "NUMBER-TEST-001";
  xcrease = "10";
  ++total;
  TestIncrease(redis, key, xcrease) ? ++ok : ++failure;

  key = "NUMBER-TEST-002";
  xcrease = "20";
  ++total;
  TestIncrease(redis, key, xcrease) ? ++ok : ++failure;

  key = "NUMBER-TEST-003";
  xcrease = "30";
  ++total;
  TestIncrease(redis, key, xcrease) ? ++ok : ++failure;

  key = "NUMBER-TEST-004";
  xcrease = "40";
  ++total;
  TestIncrease(redis, key, xcrease) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);




  key = "NUMBER-TEST-001";
  ++total;
  TestDelete(redis, key) ? ++ok : ++failure;

  key = "NUMBER-TEST-002";
  ++total;
  TestDelete(redis, key) ? ++ok : ++failure;

  key = "NUMBER-TEST-003";
  ++total;
  TestDelete(redis, key) ? ++ok : ++failure;

  key = "NUMBER-TEST-004";
  ++total;
  TestDelete(redis, key) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);




  return 0;
}

