//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-21 18:16:16.
// File name: storage_list_unit_test.cc
//
// Description: 
// Unit test of storage list request.
//

#include "common/list_message_types.h"
#include "data_proxy_server/server/server_configure.h"
#include "data_proxy_server/storage_proxy/storage.h"
#include "global/async_logging_singleton.h"

using namespace squirrel;
using namespace squirrel::dataproxy;
using namespace squirrel::dataproxy::storage;

bool TestPush(Storage &storage, const std::string &key, const std::string &element) {
  int retcode = -1;
  retcode = storage.ListPush(key, element);
  if(retcode != common::ListResponseType::LIST_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] ListPush [%s, %s]\n", key.c_str(), element.c_str());
    return false;
  }
  printf("Push key[%s] element[%s] ok\n", key.c_str(), element.c_str());
  return true;
}

bool TestPop(Storage &storage, const std::string &key) {
  int retcode = -1;
  std::string element;
  retcode = storage.ListPop(key, element);
  if(retcode != common::ListResponseType::LIST_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] ListPop [%s]\n", key.c_str());
    return false;
  }
  printf("Pop key[%s], element[%s] ok\n", key.c_str(), element.c_str());
  return true;
}

bool TestRemove(Storage &storage, const std::string &key, const std::string &element) {
  int retcode = -1;
  retcode = storage.ListRemove(key, element);
  if(retcode != common::ListResponseType::LIST_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] ListRemove [%s, %s]\n", key.c_str(), element.c_str());
    return false;
  }
  printf("Remove key[%s], element[%s] ok\n", key.c_str(), element.c_str());
  return true;
}

bool TestClear(Storage &storage, const std::string &key) {
  int retcode = -1;
  retcode = storage.ListClear(key);
  if(retcode != common::ListResponseType::LIST_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] ListClear [%s]\n", key.c_str());
    return false;
  }
  printf("Clear key[%s] ok\n", key.c_str());
  return true;
}

bool TestDelete(Storage &storage, const std::string &key) {
  int retcode = -1;
  retcode = storage.ListDelete(key);
  if(retcode != common::ListResponseType::LIST_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] ListDelete [%s]\n", key.c_str());
    return false;
  }
  printf("Delete key[%s] ok\n", key.c_str());
  return true;
}

int main(int argc, char *argv[]) {
  if(argc != 2) {
    printf("Bad argument.\n");
    return 0;
  }

  if(global::AsyncLoggingSingleton::GetAsyncLogging()->Initialize(
        std::string(argv[0]) + ".log", 64 * 1024) == false) {
    printf("Initialize logging failed.\n");
    return 0;
  }

  global::AsyncLoggingSingleton::GetAsyncLogging()->Start();

  server::ServerConfigure configure(1);
  if(configure.Load(argv[1]) == false) {
    printf("Load configure[%s] failed.\n", argv[1]);
    return 0;
  }

  Storage storage;

  const StorageConfigure::ConnectionConfigureVec vec =
    configure.storage_configure_.GetConnectionConfigure();

  if(vec.empty()) {
    printf("[Failure] Empty storage configure.\n");
    return 0;
  }

  const Connections connection = vec[0];

  if(storage.Initialize(connection.host_, connection.port_, connection.user_,
        connection.passwd_, connection.db_, configure.procedure_configure_) == false) {
    printf("[Failure] Storage initialize failed.\n");
    return 0;
  }

  if(storage.Start() == false) {
    printf("[Failure] Storage start failed.\n");
    return 0;
  }

  if(storage.CheckConnectState() == false) {
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
  TestPush(storage, key, element) ? ++ok : ++failure;

  key = "LIST-TEST-001";
  element = "ELEMENT-001-2";
  ++total;
  TestPush(storage, key, element) ? ++ok : ++failure;

  key = "LIST-TEST-001";
  element = "ELEMENT-001-3";
  ++total;
  TestPush(storage, key, element) ? ++ok : ++failure;

  key = "LIST-TEST-001";
  element = "ELEMENT-001-4";
  ++total;
  TestPush(storage, key, element) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  key = "LIST-TEST-001";
  ++total;
  TestPop(storage, key) ? ++ok : ++failure;

  key = "LIST-TEST-001";
  ++total;
  TestPop(storage, key) ? ++ok : ++failure;

  key = "LIST-TEST-001";
  ++total;
  TestPop(storage, key) ? ++ok : ++failure;

  key = "LIST-TEST-001";
  ++total;
  TestPop(storage, key) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  key = "LIST-TEST-001";
  element = "ELEMENT-001-1";
  ++total;
  TestPush(storage, key, element) ? ++ok : ++failure;

  key = "LIST-TEST-001";
  element = "ELEMENT-001-2";
  ++total;
  TestPush(storage, key, element) ? ++ok : ++failure;

  key = "LIST-TEST-001";
  element = "ELEMENT-001-3";
  ++total;
  TestPush(storage, key, element) ? ++ok : ++failure;

  key = "LIST-TEST-001";
  element = "ELEMENT-001-4";
  ++total;
  TestPush(storage, key, element) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);




  key = "LIST-TEST-001";
  element = "ELEMENT-001-1";
  ++total;
  TestRemove(storage, key, element) ? ++ok : ++failure;

  key = "LIST-TEST-001";
  element = "ELEMENT-001-2";
  ++total;
  TestRemove(storage, key, element) ? ++ok : ++failure;

  key = "LIST-TEST-001";
  element = "ELEMENT-001-3";
  ++total;
  TestRemove(storage, key, element) ? ++ok : ++failure;

  key = "LIST-TEST-001";
  element = "ELEMENT-001-4";
  ++total;
  TestRemove(storage, key, element) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);




  key = "LIST-TEST-001";
  element = "ELEMENT-001-1";
  ++total;
  TestPush(storage, key, element) ? ++ok : ++failure;

  key = "LIST-TEST-001";
  element = "ELEMENT-001-2";
  ++total;
  TestPush(storage, key, element) ? ++ok : ++failure;

  key = "LIST-TEST-001";
  element = "ELEMENT-001-3";
  ++total;
  TestPush(storage, key, element) ? ++ok : ++failure;

  key = "LIST-TEST-001";
  element = "ELEMENT-001-4";
  ++total;
  TestPush(storage, key, element) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  key = "LIST-TEST-001";
  ++total;
  TestClear(storage, key) ? ++ok : ++failure;



  key = "LIST-TEST-001";
  ++total;
  TestDelete(storage, key) ? ++ok : ++failure;


  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);


  return 0;
}
