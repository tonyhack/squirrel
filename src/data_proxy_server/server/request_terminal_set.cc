//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-04 13:07:36.
// File name: request_terminal_set.cc
//
// Description: 
// Define class RequestTerminalSet.
//

#include "data_proxy_server/server/request_terminal_set.h"

#include "core/base/auto_mutex.h"

namespace squirrel {

namespace dataproxy {

namespace server {

RequestTerminalSet::RequestTerminalSet() : sequence_(1) {}
RequestTerminalSet::~RequestTerminalSet() {}

bool RequestTerminalSet::LoginDataproxyServer(const RequestTerminalPtr &terminal) {
  if(terminal->CheckLoginState() == false && terminal->GetServerGuid() == 0) {
    core::uint64 sequence = 0;
    if(this->sequence_.Allocate(sequence) == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [RequestTerminalSet] [host=%s:%d type=%d id=%u] Allocate sequence error.",
          __FILE__, __LINE__, __FUNCTION__, terminal->GetServerHost().c_str(),
          terminal->GetServerPort(), terminal->GetServerType(), terminal->GetServerID());
      return false;
    }
    {
      terminal->SetServerGuid(sequence);
      core::AutoMutex auto_mutex(&this->mutex_);
      this->terminals_.insert(std::make_pair(sequence, terminal));
    }
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
        "[ScheduleTerminalSet] [host=%s:%d type=%d id=%u] login dataproxy server succeed.",
        terminal->GetServerHost().c_str(), terminal->GetServerPort(),
        terminal->GetServerType(), terminal->GetServerID());
    return true;
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [RequestTerminalSet] [host=%s:%d type=%d id=%u] Login error.",
        __FILE__, __LINE__, __FUNCTION__, terminal->GetServerHost().c_str(),
        terminal->GetServerPort(), terminal->GetServerType(), terminal->GetServerID());
    return false;
  }
}

bool RequestTerminalSet::LogoutDataproxyServer(const RequestTerminalPtr &terminal) {
  core::AutoMutex auto_mutex(&this->mutex_);
  TerminalMap::iterator iterator = this->terminals_.find(terminal->GetServerGuid());
  if(iterator != this->terminals_.end()) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
        "[ScheduleTerminalSet] [host=%s:%d type=%d id=%u] logout dataproxy server succeed.",
        terminal->GetServerHost().c_str(), terminal->GetServerPort(),
        terminal->GetServerType(), terminal->GetServerID());
    this->terminals_.erase(iterator);
    return true;
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [RequestTerminalSet] [host=%s:%d type=%d id=%u] Logout server error.",
        __FILE__, __LINE__, __FUNCTION__, terminal->GetServerHost().c_str(),
        terminal->GetServerPort(), terminal->GetServerType(), terminal->GetServerID());
    return false;
  }
}

RequestTerminalPtr RequestTerminalSet::GetRequestTerminal(core::uint64 guid) {
  core::AutoMutex auto_mutex(&this->mutex_);
  TerminalMap::iterator iterator = this->terminals_.find(guid);
  if(iterator != this->terminals_.end()) {
    return iterator->second;
  } else {
    return RequestTerminalPtr();
  }
}

RequestTerminalSet RequestTerminalSetSingleton::request_terminal_set_;

}  // namespace server

}  // namespace dataproxy

}  // namespace squirrel

