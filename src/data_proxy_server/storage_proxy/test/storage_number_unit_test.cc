//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-19 20:40:55.
// File name: storage_number_unit_test.cc
//
// Description: 
// Unit test of storage number request.
//

#include "common/number_message_types.h"
#include "data_proxy_server/server/server_configure.h"
#include "data_proxy_server/storage_proxy/storage.h"
#include "global/async_logging_singleton.h"

using namespace squirrel;
using namespace squirrel::dataproxy;
using namespace squirrel::dataproxy::storage;

bool TestIncrease(Storage &storage, const std::string &key, const std::string &increment) {
  int retcode = -1;
  std::string val;
  retcode = storage.NumberIncrease(key, increment, val);
  if(retcode != common::NumberResponseType::NUMBER_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] NumberIncrease [%s, %s]\n", key.c_str(), increment.c_str());
    return false;
  }
  printf("Increase key[%s] increment[%s] value[%s] ok\n", key.c_str(), increment.c_str(), val.c_str());
  return true;
}

bool TestDecrease(Storage &storage, const std::string &key, const std::string &decrement) {
  int retcode = -1;
  std::string val;
  retcode = storage.NumberDecrease(key, decrement, val);
  if(retcode != common::NumberResponseType::NUMBER_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] NumberDecrease [%s, %s]\n", key.c_str(), decrement.c_str());
    return false;
  }
  printf("Decrease key[%s] decrement[%s] value[%s] ok\n", key.c_str(), decrement.c_str(), val.c_str());
  return true;
}

bool TestReset(Storage &storage, const std::string &key) {
  int retcode = -1;
  std::string val;
  retcode = storage.NumberReset(key);
  if(retcode != common::NumberResponseType::NUMBER_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] NumberReset [%s]\n", key.c_str());
    return false;
  }
  printf("Reset key[%s] ok\n", key.c_str());
  return true;
}

bool TestDelete(Storage &storage, const std::string &key) {
  int retcode = -1;
  std::string val;
  retcode = storage.NumberDelete(key);
  if(retcode != common::NumberResponseType::NUMBER_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] NumberDelete [%s]\n", key.c_str());
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

  std::string key, xcrease;



  key = "NUMBER-TEST-001";
  xcrease = "10";
  ++total;
  TestIncrease(storage, key, xcrease) ? ++ok : ++failure;

  key = "NUMBER-TEST-002";
  xcrease = "20";
  ++total;
  TestIncrease(storage, key, xcrease) ? ++ok : ++failure;

  key = "NUMBER-TEST-003";
  xcrease = "30";
  ++total;
  TestIncrease(storage, key, xcrease) ? ++ok : ++failure;

  key = "NUMBER-TEST-004";
  xcrease = "40";
  ++total;
  TestIncrease(storage, key, xcrease) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  key = "NUMBER-TEST-001";
  xcrease = "5";
  ++total;
  TestDecrease(storage, key, xcrease) ? ++ok : ++failure;

  key = "NUMBER-TEST-002";
  xcrease = "10";
  ++total;
  TestDecrease(storage, key, xcrease) ? ++ok : ++failure;

  key = "NUMBER-TEST-003";
  xcrease = "15";
  ++total;
  TestDecrease(storage, key, xcrease) ? ++ok : ++failure;

  key = "NUMBER-TEST-004";
  xcrease = "20";
  ++total;
  TestDecrease(storage, key, xcrease) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  key = "NUMBER-TEST-001";
  ++total;
  TestReset(storage, key) ? ++ok : ++failure;

  key = "NUMBER-TEST-002";
  ++total;
  TestReset(storage, key) ? ++ok : ++failure;

  key = "NUMBER-TEST-003";
  ++total;
  TestReset(storage, key) ? ++ok : ++failure;

  key = "NUMBER-TEST-004";
  ++total;
  TestReset(storage, key) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);




  key = "NUMBER-TEST-001";
  ++total;
  TestDelete(storage, key) ? ++ok : ++failure;

  key = "NUMBER-TEST-002";
  ++total;
  TestDelete(storage, key) ? ++ok : ++failure;

  key = "NUMBER-TEST-003";
  ++total;
  TestDelete(storage, key) ? ++ok : ++failure;

  key = "NUMBER-TEST-004";
  ++total;
  TestDelete(storage, key) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);


  key = "NUMBER-TEST-001";
  xcrease = "10";
  ++total;
  TestIncrease(storage, key, xcrease) ? ++ok : ++failure;

  key = "NUMBER-TEST-002";
  xcrease = "20";
  ++total;
  TestIncrease(storage, key, xcrease) ? ++ok : ++failure;

  key = "NUMBER-TEST-003";
  xcrease = "30";
  ++total;
  TestIncrease(storage, key, xcrease) ? ++ok : ++failure;

  key = "NUMBER-TEST-004";
  xcrease = "40";
  ++total;
  TestIncrease(storage, key, xcrease) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);




  key = "NUMBER-TEST-001";
  ++total;
  TestDelete(storage, key) ? ++ok : ++failure;

  key = "NUMBER-TEST-002";
  ++total;
  TestDelete(storage, key) ? ++ok : ++failure;

  key = "NUMBER-TEST-003";
  ++total;
  TestDelete(storage, key) ? ++ok : ++failure;

  key = "NUMBER-TEST-004";
  ++total;
  TestDelete(storage, key) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  return 0;
}

