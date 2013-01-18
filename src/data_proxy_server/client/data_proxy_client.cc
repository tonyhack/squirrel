//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-10 15:29:17.
// File name: data_proxy_client.cc
//
// Description: 
// Define class DataProxyClient.
//

#include "data_proxy_server/client/data_proxy_client.h"

#include "common/message_data_proxy_types.h"
#include "common/message_types.h"
#include "global/elf_hash.h"
#include "global/time_consuming.h"

namespace squirrel {

using namespace global;

namespace dataproxy {

namespace client {

DataProxyClient *DataProxyClientSingleton::client_;

DataProxyClient::DataProxyClient() : loop_(NULL),
  host_type_(common::ServerType::SERVER_TYPE_MAX), host_id_(0) {}

DataProxyClient::~DataProxyClient() {}

bool DataProxyClient::Initialize(core::EventLoop *loop, core::uint32 server_id,
    common::ServerType::type server_type, const core::InetAddress &local_address) {
  assert(loop);
  this->loop_ = loop;
  this->host_type_ = server_type;
  this->host_id_ = server_id;
  local_address.GetAddressInfo(this->host_ip_, this->host_port_);

  return true;
}

void DataProxyClient::Start() {
}

bool DataProxyClient::AddSession(const DataProxyClientSessionPtr &session) {
  core::AutoMutex auto_mutex(&this->mutex_);
  std::pair<SessionMap::iterator, bool> ret =
    this->sessions_.insert(std::make_pair(session->GetServerID(), session));
  return ret.second;
}

bool DataProxyClient::RemoveSession(core::uint32 server_id) {
  core::AutoMutex auto_mutex(&this->mutex_);
  SessionMap::iterator iterator = this->sessions_.find(server_id);
  if(iterator != this->sessions_.end()) {
    iterator->second->Stop();
    this->sessions_.erase(iterator);
    return true;
  }
  return false;
}

void DataProxyClient::Stop() {
  SessionMap::iterator iterator = this->sessions_.begin();
  for(; iterator != this->sessions_.end(); ++iterator) {
    iterator->second->Stop();
  }
}

bool DataProxyClient::Request(common::RequestType::type request_type,
    common::StorageType::type storage_type, common::RequestRWType::type rd_type,
    const std::string &key, const std::string &request_data, core::uint64 request_id) {
#ifdef _TIME_CONSUMING_TEST
  global::TimeConsuming time_consume("DataProxyClient::Request");
#endif
  core::uint32 hash_key = global::elf_hash(key.data(), key.size());
  common::ServerSignature signature;
  if(DefaultConfigureSingleton::GetDefaultConfigure().HashGet(hash_key, signature) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [DataProxyClient] HashGet failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  {
    core::AutoMutex auto_mutex(&this->mutex_);
    // Serialization.
    SessionMap::iterator iterator = this->sessions_.find(signature.id_);
    if(iterator == this->sessions_.end()) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [DataProxyClient] Connection failure, server_id[%d].",
          __FILE__, __LINE__, __FUNCTION__, signature.id_);
      return false;
    }
    if(iterator->second->CheckLoginState() == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [DataProxyClient] Connection not logined, server_id[%d].",
          __FILE__, __LINE__, __FUNCTION__, signature.id_);
      return false;
    }
    /*
    if(this->sequence_allcator_.Allocate(request_id) == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [DataProxyClient] Allocate sequence number failure.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    */
    // Request.
    common::MessageDataProxyDataRequest request;
    request.__set_type_(request_type);
    request.__set_storage_type_(storage_type);
    request.__set_rd_type_(rd_type);
    request.__set_key_(key);
    request.__set_request_(request_data);
    request.__set_request_id_(request_id);
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
        "[DataProxyClient] [20120917] request_id[%ld]", request_id);
    size_t size = 0;
    const char *msg_ptr = this->packet_.Serialize<common::MessageDataProxyDataRequest>(
        &request, size);
    if(msg_ptr == NULL) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [DataProxyClient] Serialize MessageDataProxyDataRequest failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    common::MessageDataProxy message_dataproxy;
    message_dataproxy.__set_type_(common::MessageDataProxyType::MESSAGE_DATA_PROXY_TYPE_DATA_REQUEST);
    message_dataproxy.__isset.message_ = true;
    message_dataproxy.message_.assign(msg_ptr, size);
    msg_ptr = this->packet_.Serialize<common::MessageDataProxy>(&message_dataproxy, size);
    if(msg_ptr == NULL) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [DataProxyClient] Serialize MessageDataProxy failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    common::Message message;
    message.__set_type_(common::MessageType::MESSAGE_TYPE_DATA_PROXY);
    message.message_.assign(msg_ptr, size);
    msg_ptr = this->packet_.Serialize<common::Message>(&message, size);
    if(msg_ptr == NULL) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [DataProxyClient] Serialize Message failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    // Send request.
    iterator->second->SendMessage(msg_ptr, size);
    return true;
  }
}

void DataProxyClient::OnResponse(const common::MessageDataProxyDataResponse *response) {
  this->response_callback_(response);
}

}  // namespace client

}  // namespace dataproxy

}  // namespace squirrel


