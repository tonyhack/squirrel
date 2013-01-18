//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-14 10:36:35.
// File name: storage_reconnector_unit_test.cc
//
// Description: 
// Unit test of storage reconnector.
//

#include "data_proxy_server/server/server_configure.h"
#include "data_proxy_server/storage_proxy/storage.h"
#include "data_proxy_server/storage_proxy/storage_reconnector.h"
#include "global/async_logging_singleton.h"

using namespace squirrel;
using namespace squirrel::dataproxy;
using namespace squirrel::dataproxy::storage;

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

  const StorageConfigure::ConnectionConfigureVec &connections =
    configure.storage_configure_.GetConnectionConfigure();
  if(connections.empty()) {
    printf("Load configure[%s] empty storage.\n", argv[1]);
    return 0;
  }

  Connections connection = connections[0];

  bool Initialize(const std::string &host, core::uint16 port,
    const std::string &user, const std::string &passwd,
    const std::string &db, const ProcedureConfigure &configure);

  storage::StoragePtr storage(new (std::nothrow) Storage());
  if(storage == NULL) {
    printf("[Failure] Allocate storage error.\n");
    return 0;
  }
  if(storage->Initialize(connection.host_, connection.port_, connection.user_,
        connection.passwd_, connection.db_, configure.procedure_configure_) == false) {
    printf("[Failure] Initialize storage error.\n");
    return 0;
  }

  // storage->Start();

  StorageReconnectRequest request(storage);

  StorageReconnector reconnector;

  reconnector.Start();

  reconnector.Reconnect(request);

  sleep(10);

  reconnector.Stop();

  global::AsyncLoggingSingleton::GetAsyncLogging()->Stop();

  return 0;
}

