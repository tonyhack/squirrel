//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-29 03:59:20.
// File name: storage_map_unit_test.cc
//
// Description: 
// Unit test of storage map request.
//

#include "common/map_message_types.h"
#include "data_proxy_server/server/server_configure.h"
#include "data_proxy_server/storage_proxy/storage.h"
#include "global/async_logging_singleton.h"

using namespace squirrel;
using namespace squirrel::dataproxy;
using namespace squirrel::dataproxy::storage;

bool TestInsert(Storage &storage, const std::string &key,
    const std::string &score, const std::string &element) {
  int retcode = -1;
  retcode = storage.MapInsert(key, score, element);
  if(retcode != common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] MapInsert [%s, %s, %s]\n",
        key.c_str(), score.c_str(), element.c_str());
    return false;
  }
  printf("Insert key[%s] score[%s] element[%s] ok\n",
      key.c_str(), score.c_str(), element.c_str());
  return true;
}

bool TestRemove(Storage &storage, const std::string &key, const std::string &element) {
  int retcode = -1;
  retcode = storage.MapRemove(key, element);
  if(retcode != common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] MapRemove [%s, %s]\n", key.c_str(), element.c_str());
    return false;
  }
  printf("Remove key[%s] element[%s] ok\n", key.c_str(), element.c_str());
  return true;
}

bool TestQueryRank(Storage &storage, const std::string &key,
    const std::string &element, bool asc) {
  int retcode = -1;
  std::string ranking;
  retcode = storage.MapQueryRank(key, element, ranking, asc);
  if(retcode != common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS) {
    if(asc)
      printf("[Failure] MapQueryRank [%s, %s, asc]\n", key.c_str(), element.c_str());
    else
      printf("[Failure] MapQueryRank [%s, %s, desc]\n", key.c_str(), element.c_str());
    return false;
  }
  if(asc)
    printf("QueryRank asc key[%s] element[%s] ranking[%s] ok\n",
        key.c_str(), element.c_str(), ranking.c_str());
  else
    printf("QueryRank desc key[%s] element[%s] ranking[%s] ok\n",
        key.c_str(), element.c_str(), ranking.c_str());
  return true;
}

bool TestQueryRange(Storage &storage, const std::string &key,
    const std::string &rank_start, const std::string &rank_end, bool asc) {
  int retcode = -1;
  retcode = storage.MapQueryRange(key, rank_start, rank_end, asc);
  if(retcode != common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS) {
    if(asc)
      printf("[Failure] MapQueryRange [%s, %s, %s, asc]\n",
          key.c_str(), rank_start.c_str(), rank_end.c_str());
    else
      printf("[Failure] MapQueryRange [%s, %s, %s, desc]\n",
          key.c_str(), rank_start.c_str(), rank_end.c_str());
    return false;
  }
  int index = 0;
  std::string element;
  while(storage.MapFetchRangeValue(element, index) == true) {
    if(asc)
      printf("QueryRange asc ranking[%d] element[%s]\n", index++, element.c_str());
    else
      printf("QueryRange desc ranking[%d] element[%s]\n", index++, element.c_str());
  }
  storage.MapQuitFetchRange();
  if(index == 0) {
    if(asc)
      printf("QueryRange [%s, %s, %s, asc] emply set\n",
          key.c_str(), rank_start.c_str(), rank_end.c_str());
    else
      printf("QueryRange [%s, %s, %s, desc] emply set\n",
          key.c_str(), rank_start.c_str(), rank_end.c_str());
  }
  return true;
}

bool TestQueryRangeWithScores(Storage &storage, const std::string &key,
    const std::string &rank_start, const std::string &rank_end, bool asc) {
  int retcode = -1;
  retcode = storage.MapQueryRangeWithscores(key, rank_start, rank_end, asc);
  if(retcode != common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS) {
    if(asc)
      printf("[Failure] MapQueryRangeWithscores [%s, %s, %s, asc] retcode[%d]\n",
          key.c_str(), rank_start.c_str(), rank_end.c_str(), retcode);
    else
      printf("[Failure] MapQueryRangeWithscores [%s, %s, %s, desc] retcode[%d]\n",
          key.c_str(), rank_start.c_str(), rank_end.c_str(), retcode);
    return false;
  }
  int index = 0;
  std::string element, score;
  while(storage.MapFetchRangeValueWithscores(score, element, index) == true) {
    if(asc)
      printf("QueryRangeWithscores asc score[%s] ranking[%d] element[%s]\n",
          score.c_str(), index++, element.c_str());
    else
      printf("QueryRangeWithscores desc score[%s] ranking[%d] element[%s]\n",
          score.c_str(), index++, element.c_str());
  }
  storage.MapQuitFetchRange();
  if(index == 0) {
    if(asc)
      printf("QueryRangeWithscores [%s, %s, %s, asc] emply set\n",
          key.c_str(), rank_start.c_str(), rank_end.c_str());
    else
      printf("QueryRangeWithscores [%s, %s, %s, desc] emply set\n",
          key.c_str(), rank_start.c_str(), rank_end.c_str());
  }
  return true;
}

bool TestResetScore(Storage &storage, const std::string &key,
    const std::string &score, const std::string &element) {
  int retcode = -1;
  retcode = storage.MapResetScore(key, score, element);
  if(retcode != common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] MapResetScore [%s, %s, %s]\n",
        key.c_str(), score.c_str(), element.c_str());
    return false;
  }
  printf("ResetScore key[%s] score[%s] element[%s] ok\n",
      key.c_str(), score.c_str(), element.c_str());
  return true;
}

bool TestClear(Storage &storage, const std::string &key) {
  int retcode = -1;
  retcode = storage.MapClear(key);
  if(retcode != common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] MapClear [%s]\n", key.c_str());
    return false;
  }
  printf("Clear key[%s] ok\n", key.c_str());
  return true;
}

bool TestDelete(Storage &storage, const std::string &key) {
  int retcode = -1;
  retcode = storage.MapDelete(key);
  if(retcode != common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] MapDelete [%s]\n", key.c_str());
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

  std::string key, element, score;



  key = "MAP-TEST-001";
  element = "ELEMENT-001-1";
  score = "15";
  ++total;
  TestInsert(storage, key, score, element) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  element = "ELEMENT-001-2";
  score = "15";
  ++total;
  TestInsert(storage, key, score, element) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  element = "ELEMENT-001-3";
  score = "14";
  ++total;
  TestInsert(storage, key, score, element) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  element = "ELEMENT-001-4";
  score = "66";
  ++total;
  TestInsert(storage, key, score, element) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  key = "MAP-TEST-001";
  element = "ELEMENT-001-1";
  ++total;
  TestRemove(storage, key, element) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  element = "ELEMENT-001-2";
  ++total;
  TestRemove(storage, key, element) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  element = "ELEMENT-001-3";
  ++total;
  TestRemove(storage, key, element) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  element = "ELEMENT-001-4";
  ++total;
  TestRemove(storage, key, element) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  key = "MAP-TEST-001";
  element = "ELEMENT-001-1";
  score = "15";
  ++total;
  TestInsert(storage, key, score, element) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  element = "ELEMENT-001-2";
  score = "15";
  ++total;
  TestInsert(storage, key, score, element) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  element = "ELEMENT-001-3";
  score = "14";
  ++total;
  TestInsert(storage, key, score, element) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  element = "ELEMENT-001-4";
  score = "66";
  ++total;
  TestInsert(storage, key, score, element) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);




  key = "MAP-TEST-001";
  element = "ELEMENT-001-1";
  ++total;
  TestQueryRank(storage, key, element, true) ? ++ok : ++failure;
  ++total;
  TestQueryRank(storage, key, element, false) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  element = "ELEMENT-001-2";
  ++total;
  TestQueryRank(storage, key, element, true) ? ++ok : ++failure;
  ++total;
  TestQueryRank(storage, key, element, false) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  element = "ELEMENT-001-3";
  ++total;
  TestQueryRank(storage, key, element, true) ? ++ok : ++failure;
  ++total;
  TestQueryRank(storage, key, element, false) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  element = "ELEMENT-001-4";
  ++total;
  TestQueryRank(storage, key, element, true) ? ++ok : ++failure;
  ++total;
  TestQueryRank(storage, key, element, false) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  key = "MAP-TEST-001";
  ++total;
  TestQueryRange(storage, key, "0", "4", true) ? ++ok : ++failure;
  ++total;
  TestQueryRange(storage, key, "0", "4", false) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  ++total;
  TestQueryRange(storage, key, "1", "3", true) ? ++ok : ++failure;
  ++total;
  TestQueryRange(storage, key, "1", "3", false) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  ++total;
  TestQueryRange(storage, key, "1", "2", true) ? ++ok : ++failure;
  ++total;
  TestQueryRange(storage, key, "1", "2", false) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  ++total;
  TestQueryRange(storage, key, "2", "2", true) ? ++ok : ++failure;
  ++total;
  TestQueryRange(storage, key, "2", "2", false) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  key = "MAP-TEST-001";
  ++total;
  TestQueryRangeWithScores(storage, key, "0", "4", true) ? ++ok : ++failure;
  ++total;
  TestQueryRangeWithScores(storage, key, "0", "4", false) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  ++total;
  TestQueryRangeWithScores(storage, key, "1", "3", true) ? ++ok : ++failure;
  ++total;
  TestQueryRangeWithScores(storage, key, "1", "3", false) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  ++total;
  TestQueryRangeWithScores(storage, key, "1", "2", true) ? ++ok : ++failure;
  ++total;
  TestQueryRangeWithScores(storage, key, "1", "2", false) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  ++total;
  TestQueryRangeWithScores(storage, key, "2", "2", true) ? ++ok : ++failure;
  ++total;
  TestQueryRangeWithScores(storage, key, "2", "2", false) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  key = "MAP-TEST-001";
  element = "ELEMENT-001-1";
  score = "999";
  ++total;
  TestResetScore(storage, key, score, element) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  element = "ELEMENT-001-2";
  score = "999";
  ++total;
  TestResetScore(storage, key, score, element) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  element = "ELEMENT-001-3";
  score = "999";
  ++total;
  TestResetScore(storage, key, score, element) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  element = "ELEMENT-001-4";
  score = "999";
  ++total;
  TestResetScore(storage, key, score, element) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  key = "MAP-TEST-001";
  ++total;
  TestQueryRangeWithScores(storage, key, "0", "-1", true) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);




  key = "MAP-TEST-001";
  ++total;
  TestClear(storage, key) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  key = "MAP-TEST-001";
  ++total;
  TestQueryRangeWithScores(storage, key, "0", "-1", true) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);




  key = "MAP-TEST-001";
  ++total;
  TestDelete(storage, key) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);




  key = "MAP-TEST-001";
  ++total;
  TestQueryRangeWithScores(storage, key, "0", "-1", true) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  return 0;
}

