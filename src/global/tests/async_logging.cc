//
// This is a test of AsyncLogging.
//

#include <time.h>

#include "global/async_logging.h"

void Logging(squirrel::global::AsyncLogging *logging) {
  if(logging) {
    for(int pos = 0; pos < 100; ++pos) {
      logging->Logging(squirrel::global::LOGGING_LEVEL_DEBUG, "[LOGGING TEST] [%d] abcdefghijklmnopqrstuvwxyzd0123456789", pos);
      struct timespec ts = { 0, pos*500*1000 };
      nanosleep(&ts, NULL);
    }
    for(int pos = 0; pos < 100; ++pos) {
      logging->Logging(squirrel::global::LOGGING_LEVEL_WARNING, "[LOGGING TEST] [%d] abcdefghijklmnopqrstuvwxyzd0123456789", pos);
      struct timespec ts = { 0, 2*pos*500*1000 };
      nanosleep(&ts, NULL);
    }
    for(int pos = 0; pos < 100; ++pos) {
      logging->Logging(squirrel::global::LOGGING_LEVEL_INFO, "[LOGGING TEST] [%d] abcdefghijklmnopqrstuvwxyzd0123456789", pos);
      struct timespec ts = { 0, 3*pos*500*1000 };
      nanosleep(&ts, NULL);
    }
    for(int pos = 0; pos < 100; ++pos) {
      logging->Logging(squirrel::global::LOGGING_LEVEL_ERROR, "[LOGGING TEST] [%d] abcdefghijklmnopqrstuvwxyzd0123456789", pos);
      struct timespec ts = { 0, 4*pos*500*1000 };
      nanosleep(&ts, NULL);
    }
    for(int pos = 0; pos < 100; ++pos) {
      logging->Logging(squirrel::global::LOGGING_LEVEL_FATAL, "[LOGGING TEST] [%d] abcdefghijklmnopqrstuvwxyzd0123456789", pos);
      struct timespec ts = { 0, 5*pos*500*1000 };
      nanosleep(&ts, NULL);
    }
  }
}

int main(int argc, char* argv[]) {
  squirrel::global::AsyncLogging logging;

  char name[256];
  strncpy(name, argv[0], 256);

  std::cout << name << std::endl;

  if(logging.Initialize(name, 1024) == false) {
    std::cout << "squirrel::global::AsyncLogging Initialize failed!" << std::endl;
    return 0;
  }

  logging.SetLoggingLevel(squirrel::global::LOGGING_LEVEL_DEBUG);

  logging.Start();

  Logging(&logging);

  logging.Stop();

  return 0;
}

