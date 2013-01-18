//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-26 16:18:57.
// File name: redis_map_unit_test.cc
//
// Description: 
// Unit test of redis map request.
//

#include "common/map_message_types.h"
#include "data_proxy_server/redis_proxy/redis.h"
#include "global/async_logging_singleton.h"

using namespace squirrel;
using namespace squirrel::dataproxy;
using namespace squirrel::dataproxy::redis;

bool TestInsert(Redis &redis, const std::string &key,
    const std::string &score, const std::string &element) {
  int retcode = -1;
  retcode = redis.MapInsert(key, score, element);
  if(retcode != common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] MapInsert [%s, %s, %s]\n",
        key.c_str(), score.c_str(), element.c_str());
    return false;
  }
  printf("Insert key[%s] score[%s] element[%s] ok\n",
      key.c_str(), score.c_str(), element.c_str());
  return true;
}

bool TestRemove(Redis &redis, const std::string &key, const std::string &element) {
  int retcode = -1;
  retcode = redis.MapRemove(key, element);
  if(retcode != common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] MapRemove [%s, %s]\n", key.c_str(), element.c_str());
    return false;
  }
  printf("Remove key[%s] element[%s] ok\n", key.c_str(), element.c_str());
  return true;
}

bool TestQueryRank(Redis &redis, const std::string &key,
    const std::string &element, bool asc) {
  int retcode = -1;
  std::string ranking;
  retcode = redis.MapQueryRank(key, element, ranking, asc);
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

bool TestQueryRange(Redis &redis, const std::string &key,
    const std::string &rank_start, const std::string &rank_end, bool asc) {
  int retcode = -1;
  retcode = redis.MapQueryRange(key, rank_start, rank_end, asc);
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
  while(redis.MapFetchRangeValue(element, index) == true) {
    if(asc)
      printf("QueryRange asc ranking[%d] element[%s]\n", index++, element.c_str());
    else
      printf("QueryRange desc ranking[%d] element[%s]\n", index++, element.c_str());
  }
  redis.MapQuitFetchRange();
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

bool TestQueryRangeWithScores(Redis &redis, const std::string &key,
    const std::string &rank_start, const std::string &rank_end, bool asc) {
  int retcode = -1;
  retcode = redis.MapQueryRangeWithscores(key, rank_start, rank_end, asc);
  if(retcode != common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS) {
    if(asc)
      printf("[Failure] MapQueryRangeWithscores [%s, %s, %s, asc]\n",
          key.c_str(), rank_start.c_str(), rank_end.c_str());
    else
      printf("[Failure] MapQueryRangeWithscores [%s, %s, %s, desc]\n",
          key.c_str(), rank_start.c_str(), rank_end.c_str());
    return false;
  }
  int index = 0;
  std::string element, score;
  while(redis.MapFetchRangeValueWithscores(score, element, index) == true) {
    if(asc)
      printf("QueryRangeWithscores asc score[%s] ranking[%d] element[%s]\n",
          score.c_str(), index++, element.c_str());
    else
      printf("QueryRangeWithscores desc score[%s] ranking[%d] element[%s]\n",
          score.c_str(), index++, element.c_str());
  }
  redis.MapQuitFetchRange();
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

bool TestResetScore(Redis &redis, const std::string &key,
    const std::string &score, const std::string &element) {
  int retcode = -1;
  retcode = redis.MapResetScore(key, score, element);
  if(retcode != common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] MapResetScore [%s, %s, %s]\n",
        key.c_str(), score.c_str(), element.c_str());
    return false;
  }
  printf("ResetScore key[%s] score[%s] element[%s] ok\n",
      key.c_str(), score.c_str(), element.c_str());
  return true;
}

bool TestClear(Redis &redis, const std::string &key) {
  int retcode = -1;
  retcode = redis.MapClear(key);
  if(retcode != common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] MapClear [%s]\n", key.c_str());
    return false;
  }
  printf("Clear key[%s] ok\n", key.c_str());
  return true;
}

bool TestDelete(Redis &redis, const std::string &key) {
  int retcode = -1;
  retcode = redis.MapDelete(key);
  if(retcode != common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS) {
    printf("[Failure] MapDelete [%s]\n", key.c_str());
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

  std::string key, element, score;




  key = "MAP-TEST-001";
  element = "ELEMENT-001-1";
  score = "15";
  ++total;
  TestInsert(redis, key, score, element) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  element = "ELEMENT-001-2";
  score = "15";
  ++total;
  TestInsert(redis, key, score, element) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  element = "ELEMENT-001-3";
  score = "14";
  ++total;
  TestInsert(redis, key, score, element) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  element = "ELEMENT-001-4";
  score = "66";
  ++total;
  TestInsert(redis, key, score, element) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  key = "MAP-TEST-001";
  element = "ELEMENT-001-1";
  ++total;
  TestRemove(redis, key, element) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  element = "ELEMENT-001-2";
  ++total;
  TestRemove(redis, key, element) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  element = "ELEMENT-001-3";
  ++total;
  TestRemove(redis, key, element) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  element = "ELEMENT-001-4";
  ++total;
  TestRemove(redis, key, element) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  key = "MAP-TEST-001";
  element = "ELEMENT-001-1";
  score = "15";
  ++total;
  TestInsert(redis, key, score, element) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  element = "ELEMENT-001-2";
  score = "15";
  ++total;
  TestInsert(redis, key, score, element) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  element = "ELEMENT-001-3";
  score = "14";
  ++total;
  TestInsert(redis, key, score, element) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  element = "ELEMENT-001-4";
  score = "66";
  ++total;
  TestInsert(redis, key, score, element) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);




  key = "MAP-TEST-001";
  element = "ELEMENT-001-1";
  ++total;
  TestQueryRank(redis, key, element, true) ? ++ok : ++failure;
  ++total;
  TestQueryRank(redis, key, element, false) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  element = "ELEMENT-001-2";
  ++total;
  TestQueryRank(redis, key, element, true) ? ++ok : ++failure;
  ++total;
  TestQueryRank(redis, key, element, false) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  element = "ELEMENT-001-3";
  ++total;
  TestQueryRank(redis, key, element, true) ? ++ok : ++failure;
  ++total;
  TestQueryRank(redis, key, element, false) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  element = "ELEMENT-001-4";
  ++total;
  TestQueryRank(redis, key, element, true) ? ++ok : ++failure;
  ++total;
  TestQueryRank(redis, key, element, false) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  key = "MAP-TEST-001";
  ++total;
  TestQueryRange(redis, key, "0", "4", true) ? ++ok : ++failure;
  ++total;
  TestQueryRange(redis, key, "0", "4", false) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  ++total;
  TestQueryRange(redis, key, "1", "3", true) ? ++ok : ++failure;
  ++total;
  TestQueryRange(redis, key, "1", "3", false) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  ++total;
  TestQueryRange(redis, key, "1", "2", true) ? ++ok : ++failure;
  ++total;
  TestQueryRange(redis, key, "1", "2", false) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  ++total;
  TestQueryRange(redis, key, "2", "2", true) ? ++ok : ++failure;
  ++total;
  TestQueryRange(redis, key, "2", "2", false) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  key = "MAP-TEST-001";
  ++total;
  TestQueryRangeWithScores(redis, key, "0", "4", true) ? ++ok : ++failure;
  ++total;
  TestQueryRangeWithScores(redis, key, "0", "4", false) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  ++total;
  TestQueryRangeWithScores(redis, key, "1", "3", true) ? ++ok : ++failure;
  ++total;
  TestQueryRangeWithScores(redis, key, "1", "3", false) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  ++total;
  TestQueryRangeWithScores(redis, key, "1", "2", true) ? ++ok : ++failure;
  ++total;
  TestQueryRangeWithScores(redis, key, "1", "2", false) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  ++total;
  TestQueryRangeWithScores(redis, key, "2", "2", true) ? ++ok : ++failure;
  ++total;
  TestQueryRangeWithScores(redis, key, "2", "2", false) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  key = "MAP-TEST-001";
  element = "ELEMENT-001-1";
  score = "999";
  ++total;
  TestResetScore(redis, key, score, element) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  element = "ELEMENT-001-2";
  score = "999";
  ++total;
  TestResetScore(redis, key, score, element) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  element = "ELEMENT-001-3";
  score = "999";
  ++total;
  TestResetScore(redis, key, score, element) ? ++ok : ++failure;

  key = "MAP-TEST-001";
  element = "ELEMENT-001-4";
  score = "999";
  ++total;
  TestResetScore(redis, key, score, element) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  key = "MAP-TEST-001";
  ++total;
  TestQueryRangeWithScores(redis, key, "0", "-1", true) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);




  key = "MAP-TEST-001";
  ++total;
  TestClear(redis, key) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  key = "MAP-TEST-001";
  ++total;
  TestQueryRangeWithScores(redis, key, "0", "-1", true) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);




  key = "MAP-TEST-001";
  ++total;
  TestDelete(redis, key) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);



  key = "MAP-TEST-001";
  ++total;
  TestQueryRangeWithScores(redis, key, "0", "-1", true) ? ++ok : ++failure;

  printf("[Result] total=%d ok=%d failure=%d\n", total, ok, failure);




  return 0;
}

