//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-13 11:24:20.
// File name: redis_reconnector_unit_test.cc
//
// Description: 
// Unit test of redis reconnector.
//

#include "data_proxy_server/redis_proxy/redis.h"
#include "data_proxy_server/redis_proxy/redis_reconnector.h"
#include "global/async_logging_singleton.h"

using namespace squirrel;
using namespace squirrel::dataproxy;
using namespace squirrel::dataproxy::redis;

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

  redis::RedisPtr redis(new (std::nothrow) Redis());
  if(redis == NULL) {
    printf("[Failure] Allocate redis error.\n");
    return false;
  }
  if(redis->Initialize(argv[1], port) == false) {
    printf("[Failure] Initialize redis error.\n");
    return false;
  }

  RedisReconnector reconnector;
  RedisReconnectRequest request(redis);

  reconnector.Start();

  reconnector.Reconnect(request);

  sleep(10);

  reconnector.Stop();

  global::AsyncLoggingSingleton::GetAsyncLogging()->Stop();

  return 0;
}

