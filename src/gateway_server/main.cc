//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-10 20:20:38.
// File name: main.cc
//
// Description: 
// main
//

#include <libgen.h>
#include <signal.h>

#include <boost/bind.hpp>

#include "core/configure.h"
#include "core/event_loop.h"
#include "core/signal_set.h"
#include "core/base/logging.h"
#include "gateway_server/server_configure.h"
#include "gateway_server/gateway_server.h"
#include "global/async_logging_singleton.h"

using namespace squirrel;
using namespace squirrel::gateway;
using namespace squirrel::global;

void OnTerminal(core::EventLoop *loop) {
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO, "Gateway server terminal!");
  loop->Quit();
}

void MainLoop() {
  core::EventLoop loop;
}

int main(int argc, char *argv[]) {
  typedef std::map<std::string, std::string> ArgsList;
  ArgsList args;
  int pos = 1;
  while(pos + 1 < argc) {
    args.insert(std::make_pair(argv[pos], argv[pos + 1]));
    ++pos;
  }

  ArgsList::iterator iterator = args.find("-id");
  if(iterator == args.end()) {
    std::cout << "Need argument -id [number]." << std::endl;
    return 0;
  }

  int server_id = atoi(iterator->second.c_str());

  iterator = args.find("-configure");
  if(iterator == args.end()) {
    std::cout << "Need argument -configure [configure file]." << std::endl;
    return 0;
  }

  std::string configure_file = iterator->second;

  bool daemon_flag = false;

  iterator = args.find("-daemon");
  if(iterator != args.end()) {
    if(iterator->second == "true") {
      daemon_flag = true;
    }
  }

  char server_log_name[64];
  snprintf(server_log_name, sizeof(server_log_name), "%s-%d", ::basename(argv[0]), server_id);

  ServerConfigure configure(server_id);
  if(configure.Load(configure_file.c_str()) == false) {
    std::cout << "Load " << configure_file << " failed." << std::endl;
    return 0;
  }

  core::Configure::Init(configure.core_configure_);
  if(Configure::CheckFinishLoad() == false) {
    std::cout << "load configure error!!" << std::endl;
    return 0;
  }

  core::EventLoop loop;
  if(loop.Initialize(false) == false) {
    std::cout << "Initialize event loop failed." << std::endl;
    return 0;
  }

  if(daemon_flag == true) {
    if(daemon(1, 1) != 0) {
      std::cout << "failed to daemon." << std::endl;
      return 0;
    }
  }

  std::cout << "server_log_name=" << server_log_name << std::endl;

  std::string core_log_file = configure.core_log_ + server_log_name + ".log";
  core::InitializeLogger(core_log_file, core_log_file, core_log_file);

  GatewayServer server(core::InetAddress(configure.ip_, configure.port_), "GatewayServer");
  if(server.Initialize(configure, server_log_name) == false) {
    std::cout << "Initialize gateway server failed." << std::endl;
    return 0;
  }

  server.Start();

  core::SignalSet::GetSingleton()->Insert(SIGTERM, boost::bind(OnTerminal, &loop));

  loop.Loop();

  server.Stop();

  return 0;
}

