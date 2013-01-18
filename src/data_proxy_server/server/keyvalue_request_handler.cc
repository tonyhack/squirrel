//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-05 17:27:38.
// File name: keyvalue_request_handler.cc
//
// Description: 
// Define class KeyvalueRequestHandler.
//

#include "data_proxy_server/server/keyvalue_request_handler.h"

#include "common/keyvalue_message_types.h"
#include "data_proxy_server/server/async_request_loop.h"
#include "data_proxy_server/server/request_service.h"
#include "global/async_logging_singleton.h"
#include "global/time_consuming.h"

namespace squirrel {

namespace dataproxy {

namespace server {

KeyvalueRequestHandler::KeyvalueRequestHandler() : async_request_loop_(NULL) {}
KeyvalueRequestHandler::~KeyvalueRequestHandler() {}

bool KeyvalueRequestHandler::Initialize(AsyncRequestLoop *async_request_loop_ptr,
      const RedisConfigure &redis_configure, const StorageConfigure &storage_configure,
      const ProcedureConfigure &procedure_configure) {
  this->async_request_loop_ = async_request_loop_ptr;
  if(this->redises_.Initialize(redis_configure) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [KeyvalueRequestHandler] Initialize RedisSet failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  if(this->redises_.Start() == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [KeyvalueRequestHandler] Start RedisSet failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  if(this->storages_.Initialize(storage_configure, procedure_configure) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [KeyvalueRequestHandler] Initialize StorageSet failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  if(this->storages_.Start() == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [KeyvalueRequestHandler] Start StorageSet failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  return true;
}

void KeyvalueRequestHandler::Request(const common::AsyncRequestObject *object) {
#ifdef _TIME_CONSUMING_TEST
  global::TimeConsuming time_consume("KeyvalueRequestHandler::Request");
#endif
  common::KeyvalueRequest request;
  if(this->packet_.Deserialize<common::KeyvalueRequest>(
        &request, object->request_.data(), object->request_.size()) == true) {
    switch(request.type_) {
      case common::KeyvalueRequestType::KEYVALUE_REQUEST_TYPE_SET:
        this->OnRequestSet(object, request.value_);
        break;
      case common::KeyvalueRequestType::KEYVALUE_REQUEST_TYPE_GET:
        this->OnRequestGet(object);
        break;
      case common::KeyvalueRequestType::KEYVALUE_REQUEST_TYPE_DELETE:
        this->OnRequestDelete(object);
        break;
      case common::KeyvalueRequestType::KEYVALUE_REQUEST_TYPE_SYNC_REDIS:
        this->OnRequestSynchronousRedis(object);
        break;
      default:
        this->OnRequestDefault(object);
        break;
    }
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [KeyvalueRequestHandler] Deserialize KeyvalueRequest failed.",
        __FILE__, __LINE__, __FUNCTION__);
    this->OnRequestDefault(object);
  }
}

void KeyvalueRequestHandler::OnRequestGet(const common::AsyncRequestObject *object) {
  common::KeyvalueResponse response;
  response.type_ = common::KeyvalueRequestType::KEYVALUE_REQUEST_TYPE_GET;
  int retcode = common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_UNKNOWN;
  bool finish = false;
  // 1. Redis.
  if(object->storage_type_ & common::StorageType::STORAGE_TYPE_CACHE) {
    // Get from redis.
    redis::RedisPtr redis = this->redises_.GetRedis(object->key_hash_value_);
    assert(redis);
    if(redis->CheckConnectState() == true) {
      retcode = redis->KeyvalueGet(object->key_, response.value_);
      if(retcode == common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_SUCCESS) {
        response.__isset.value_ = true;
        finish = true;
      } else {
        // Get failed from redis.
        if(retcode == redis::Redis::REDIS_SERIOUS_ERROR) {
          retcode = common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_DISCONNECTED;
          // TODO: Reconnect redis.
          RequestServiceSingleton::GetRequestService()->ReconnectRedisServer(redis);
        } else {
          this->SynchronousRedis(object);
        }
      }
    } else {
      retcode = common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_DISCONNECTED;
    }
  }
  // 2. Mysql.
  if(finish == false && (object->storage_type_ & common::StorageType::STORAGE_TYPE_DISK)) {
    storage::StoragePtr storage = this->storages_.GetStorage(object->key_hash_value_);
    assert(storage);
    if(storage->CheckConnectState() == true) {
      retcode = storage->KeyvalueGet(object->key_, response.value_);
      if(retcode == common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_SUCCESS) {
        response.__isset.value_ = true;
        finish = true;
      } else {
        // Get failed from mysql.
        if(retcode == storage::Storage::STORAGE_SERIOUS_ERROR) {
          retcode = common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_DISCONNECTED;
          // TODO: Reconnect mysql.
          RequestServiceSingleton::GetRequestService()->ReconnectStorageServer(storage);
        }
      }
    } else {
      retcode = common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_DISCONNECTED;
    }
  }
  // 3. Response result.
  response.__set_result_((common::KeyvalueResponseType::type)retcode);
  this->async_request_loop_->SendResponse<common::KeyvalueResponse>(object->request_id_,
      object->terminal_guid_, object->type_, object->key_, &response);
}

void KeyvalueRequestHandler::OnRequestSet(const common::AsyncRequestObject *object,
    const std::string &value) {
  common::KeyvalueResponse response;
  response.type_ = common::KeyvalueRequestType::KEYVALUE_REQUEST_TYPE_SET;
  int retcode = common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_SUCCESS;
  // 1. Mysql.
  if(object->storage_type_ & common::StorageType::STORAGE_TYPE_DISK) {
    storage::StoragePtr storage = this->storages_.GetStorage(object->key_hash_value_);
    assert(storage);
    if(storage->CheckConnectState() == true) {
      retcode = storage->KeyvalueSet(object->key_, value);
      if(retcode == storage::Storage::STORAGE_SERIOUS_ERROR) {
        retcode = common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_DISCONNECTED;
        // TODO: Reconnect mysql.
        RequestServiceSingleton::GetRequestService()->ReconnectStorageServer(storage);
      }
    } else {
      retcode = common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_DISCONNECTED;
    }
  }
  // 2. Redis.
  if((object->storage_type_ & common::StorageType::STORAGE_TYPE_CACHE) &&
      retcode == common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_SUCCESS) {
    redis::RedisPtr redis = this->redises_.GetRedis(object->key_hash_value_);
    assert(redis);
    if(redis->CheckConnectState() == true) {
      retcode = redis->KeyvalueSet(object->key_, value);
      if(retcode == redis::Redis::REDIS_SERIOUS_ERROR) {
        retcode = common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_DISCONNECTED;
        // TODO: Reconnect redis.
        RequestServiceSingleton::GetRequestService()->ReconnectRedisServer(redis);
      }
    } else {
      retcode = common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_DISCONNECTED;
    }
  }
  // 3. Response result.
  response.__set_result_((common::KeyvalueResponseType::type)retcode);
  this->async_request_loop_->SendResponse<common::KeyvalueResponse>(object->request_id_,
      object->terminal_guid_, object->type_, object->key_, &response);
}

void KeyvalueRequestHandler::OnRequestDelete(const common::AsyncRequestObject *object) {
  common::KeyvalueResponse response;
  response.type_ = common::KeyvalueRequestType::KEYVALUE_REQUEST_TYPE_DELETE;
  int retcode = common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_SUCCESS;
  // 1. Mysql.
  if(object->storage_type_ & common::StorageType::STORAGE_TYPE_DISK) {
    storage::StoragePtr storage = this->storages_.GetStorage(object->key_hash_value_);
    assert(storage);
    if(storage->CheckConnectState() == true) {
      retcode = storage->KeyvalueDelete(object->key_);
      if(retcode == storage::Storage::STORAGE_SERIOUS_ERROR) {
        retcode = common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_DISCONNECTED;
        // TODO: Reconnect mysql.
        RequestServiceSingleton::GetRequestService()->ReconnectStorageServer(storage);
      }
    } else {
      retcode = common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_DISCONNECTED;
    }
  }
  // 2. Redis.
  if((object->storage_type_ & common::StorageType::STORAGE_TYPE_CACHE) &&
      retcode == common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_SUCCESS) {
    redis::RedisPtr redis = this->redises_.GetRedis(object->key_hash_value_);
    assert(redis);
    if(redis->CheckConnectState() == true) {
      retcode = redis->KeyvalueDelete(object->key_);
      if(retcode == redis::Redis::REDIS_SERIOUS_ERROR) {
        retcode = common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_DISCONNECTED;
        // TODO: Reconnect redis.
        RequestServiceSingleton::GetRequestService()->ReconnectRedisServer(redis);
      }
    } else {
      retcode = common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_DISCONNECTED;
    }
  }
  // 3. Response result.
  response.__set_result_((common::KeyvalueResponseType::type)retcode);
  this->async_request_loop_->SendResponse<common::KeyvalueResponse>(object->request_id_,
      object->terminal_guid_, object->type_, object->key_, &response);
}

void KeyvalueRequestHandler::OnRequestSynchronousRedis(const common::AsyncRequestObject *object) {
  std::string value;
  // Get from mysql.
  storage::StoragePtr storage = this->storages_.GetStorage(object->key_hash_value_);
  assert(storage);
  if(storage->CheckConnectState() == true) {
    int retcode = storage->KeyvalueGet(object->key_, value);
    if(retcode == common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_SUCCESS) {
      // Set to redis.
      redis::RedisPtr redis = this->redises_.GetRedis(object->key_hash_value_);
      assert(redis);
      if(redis->CheckConnectState() == true) {
        if(redis->KeyvalueSet(object->key_, value) == redis::Redis::REDIS_SERIOUS_ERROR) {
          // TODO: Reconnect redis.
          RequestServiceSingleton::GetRequestService()->ReconnectRedisServer(redis);
        }
      }
    } else if(retcode == storage::Storage::STORAGE_SERIOUS_ERROR) {
      // TODO: Reconnect mysql.
      RequestServiceSingleton::GetRequestService()->ReconnectStorageServer(storage);
    }
  }
}

void KeyvalueRequestHandler::OnRequestDefault(const common::AsyncRequestObject *object) {
  common::KeyvalueResponse response;
  response.__set_result_(common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_UNKNOWN);
  this->async_request_loop_->SendResponse<common::KeyvalueResponse>(object->request_id_,
      object->terminal_guid_, object->type_, object->key_, &response);
}

void KeyvalueRequestHandler::SynchronousRedis(const common::AsyncRequestObject *object) {
  // request
  common::KeyvalueRequest request;
  size_t size = 0;
  request.__set_type_(common::KeyvalueRequestType::KEYVALUE_REQUEST_TYPE_SYNC_REDIS);
  const char *msg_ptr = this->packet_.Serialize<common::KeyvalueRequest>(&request, size);
  if(msg_ptr) {
    common::AsyncRequestObject sync_object;
    sync_object.__set_type_(common::RequestType::REQUEST_TYPE_KEYVALUE);
    sync_object.__set_storage_type_(common::StorageType::type(common::StorageType::STORAGE_TYPE_CACHE |
        common::StorageType::STORAGE_TYPE_DISK));
    sync_object.__set_key_(object->key_);
    sync_object.__set_key_hash_value_(object->storage_type_);
    sync_object.request_.assign(msg_ptr, size);
    // Synchronous request.
    RequestServiceSingleton::GetRequestService()->Request(
        common::RequestRWType::REQUEST_RW_TYPE_WRITE, &sync_object);
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [KeyvalueRequestHandler] Serialize KeyvalueRequest failed.",
        __FILE__, __LINE__, __FUNCTION__);
  }
}

}  // namespace server

}  // namespace dataproxy

}  // namespace squirrel

