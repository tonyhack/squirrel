#include <boost/bind.hpp>

#include "core/event_loop.h"
#include "core/signal_set.h"

void OnSignalTermHandler() {
  printf("SignalHandler key[SIGTERM]\n");
}

void OnSignalQuitHandler() {
  printf("SignalHandler key[SIGQUIT]\n");
}

void OnSignalIntHandler() {
  printf("SignalHandler key[SIGINT]\n");
  exit(0);
}

int main() {
  core::SignalSet::GetSingleton()->Insert(SIGTERM, boost::bind(OnSignalTermHandler));
  core::SignalSet::GetSingleton()->Insert(SIGQUIT, boost::bind(OnSignalQuitHandler));
  core::SignalSet::GetSingleton()->Insert(SIGINT, boost::bind(OnSignalIntHandler));

  core::EventLoop loop;
  loop.Initialize(false);

  loop.Loop();

  return 0;
}

