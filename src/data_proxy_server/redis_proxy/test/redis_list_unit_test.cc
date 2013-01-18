//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-21 17:13:31.
// File name: redis_list_unit_test.cc
//
// Description: 
// Unit test of redis list request.
//

#include "common/list_message_types.h"
#include "data_proxy_server/redis_proxy/redis.h"
#include "global/async_logging_singleton.h"

using namespace squirrel;
using namespace squirrel::dataproxy;
using namespace squirrel::dataproxy::redis;


bool TestPush(Redis &redis, const std::string &key, const std::string &element) {
  int retcode = -1;
  retcode = redis.ListPush(key, element);
  if(retcode != common::ListResponseType::LIST_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] ListPush [%s, %s]\n", key.c_str(), element.c_str());
    return false;
  }
  printf("Push key[%s] element[%s] ok\n", key.c_str(), element.c_str());
  return true;
}

bool TestPop(Redis &redis, const std::string &key) {
  int retcode = -1;
  std::string element;
  retcode = redis.ListPop(key, element);
  if(retcode != common::ListResponseType::LIST_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] ListPop [%s] error.\n", key.c_str());
    return false;
  } else {
    printf("Pop key[%s, %s] ok\n", key.c_str(), element.c_str());
    return true;
  }
}

bool TestRemove(Redis &redis, const std::string &key, const std::string &element) {
  int retcode = -1;
  retcode = redis.ListRemove(key, element);
  if(retcode != common::ListResponseType::LIST_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] ListRemove [%s, %s] error.\n", key.c_str(), element.c_str());
    return false;
  } else {
    printf("Remove key[%s] element[%s] ok\n", key.c_str(), element.c_str());
    return true;
  }
}

bool TestClear(Redis &redis, const std::string &key) {
  int retcode = -1;
  retcode = redis.ListClear(key);
  if(retcode != common::ListResponseType::LIST_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] ListClear [%s] error.\n", key.c_str());
    return false;
  } else {
    printf("Clear key[%s] ok\n", key.c_str());
    return true;
  }
}

bool TestDelete(Redis &redis, const std::string &key) {
  int retcode = -1;
  retcode = redis.ListDelete(key);
  if(retcode != common::ListResponseType::LIST_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] ListDelete [%s] error.\n", key.c_str());
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

  std::string key, element;




  key = "LIST-TEST-001";
  element = "ELEMENT-001-1";
  ++total;
  TestPush(redis, key, element) ? ++ok : ++failure;

  key = "LIST-TEST-001";
  element = "ELEMENT-001-2";
  ++total;
  TestPush(redis, key, element) ? ++ok : ++failure;

  key = "LIST-TEST-001";
  element = "ELEMENT-001-3";
  ++total;
  TestPush(redis, key, element) ? ++ok : ++failure;

  key = "LIST-TEST-001";
  element = "ELEMENT-001-4";
  ++total;
  TestPush(redis, key, element) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  key = "LIST-TEST-001";
  ++total;
  TestPop(redis, key) ? ++ok : ++failure;

  key = "LIST-TEST-001";
  ++total;
  TestPop(redis, key) ? ++ok : ++failure;

  key = "LIST-TEST-001";
  ++total;
  TestPop(redis, key) ? ++ok : ++failure;

  key = "LIST-TEST-001";
  ++total;
  TestPop(redis, key) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  key = "LIST-TEST-001";
  element = "ELEMENT-001-1";
  ++total;
  TestPush(redis, key, element) ? ++ok : ++failure;

  key = "LIST-TEST-001";
  element = "ELEMENT-001-2";
  ++total;
  TestPush(redis, key, element) ? ++ok : ++failure;

  key = "LIST-TEST-001";
  element = "ELEMENT-001-3";
  ++total;
  TestPush(redis, key, element) ? ++ok : ++failure;

  key = "LIST-TEST-001";
  element = "ELEMENT-001-4";
  ++total;
  TestPush(redis, key, element) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);




  key = "LIST-TEST-001";
  element = "ELEMENT-001-1";
  ++total;
  TestRemove(redis, key, element) ? ++ok : ++failure;

  key = "LIST-TEST-001";
  element = "ELEMENT-001-2";
  ++total;
  TestRemove(redis, key, element) ? ++ok : ++failure;

  key = "LIST-TEST-001";
  element = "ELEMENT-001-3";
  ++total;
  TestRemove(redis, key, element) ? ++ok : ++failure;

  key = "LIST-TEST-001";
  element = "ELEMENT-001-4";
  ++total;
  TestRemove(redis, key, element) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);




  key = "LIST-TEST-001";
  element = "ELEMENT-001-1";
  ++total;
  TestPush(redis, key, element) ? ++ok : ++failure;

  key = "LIST-TEST-001";
  element = "ELEMENT-001-2";
  ++total;
  TestPush(redis, key, element) ? ++ok : ++failure;

  key = "LIST-TEST-001";
  element = "ELEMENT-001-3";
  ++total;
  TestPush(redis, key, element) ? ++ok : ++failure;

  key = "LIST-TEST-001";
  element = "ELEMENT-001-4";
  ++total;
  TestPush(redis, key, element) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  key = "LIST-TEST-001";
  ++total;
  TestClear(redis, key) ? ++ok : ++failure;


/*
  key = "LIST-TEST-001";
  ++total;
  TestDelete(redis, key) ? ++ok : ++failure;
*/


  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  return 0;
}

