//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-21 20:50:50.
// File name: list_request_handler.cc
//
// Description: 
// Define class ListRequestHandler.
//

#include "data_proxy_server/server/list_request_handler.h"

#include "common/list_message_types.h"
#include "data_proxy_server/server/async_request_loop.h"
#include "data_proxy_server/server/request_service.h"
#include "global/async_logging_singleton.h"
#include "global/time_consuming.h"

namespace squirrel {

namespace dataproxy {

namespace server {

ListRequestHandler::ListRequestHandler() : async_request_loop_(NULL) {}
ListRequestHandler::~ListRequestHandler() {}

bool ListRequestHandler::Initialize(AsyncRequestLoop *async_request_loop_ptr,
    const RedisConfigure &redis_configure, const StorageConfigure &storage_configure,
    const ProcedureConfigure &procedure_configure) {
  this->async_request_loop_ = async_request_loop_ptr;
  if(this->redises_.Initialize(redis_configure) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [ListRequestHandler] Initialize RedisSet failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  if(this->redises_.Start() == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [ListRequestHandler] Start RedisSet failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  if(this->storages_.Initialize(storage_configure, procedure_configure) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [ListRequestHandler] Initialize StorageSet failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  if(this->storages_.Start() == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [ListRequestHandler] Start StorageSet failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  return true;
}

void ListRequestHandler::Request(const common::AsyncRequestObject *object) {
#ifdef _TIME_CONSUMING_TEST
  global::TimeConsuming time_consume("ListRequestHandler::Request");
#endif
  common::ListRequest request;
  if(this->packet_.Deserialize<common::ListRequest>(
        &request, object->request_.data(), object->request_.size()) == true) {
    switch(request.type_) {
      case common::ListRequestType::LIST_REQUEST_TYPE_PUSH:
        this->OnRequestPush(object, request.data_);
        break;
      case common::ListRequestType::LIST_REQUEST_TYPE_POP:
        this->OnRequestPop(object);
        break;
      case common::ListRequestType::LIST_REQUEST_TYPE_REMOVE:
        this->OnRequestRemove(object, request.data_);
        break;
      case common::ListRequestType::LIST_REQUEST_TYPE_CLEAR:
        this->OnRequestClear(object);
        break;
      case common::ListRequestType::LIST_REQUEST_TYPE_DELETE:
        this->OnRequestDelete(object);
        break;
      default:
        this->OnRequestDefault(object);
        break;
    }
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [ListRequestHandler] Deserialize KeyvalueRequest failed.",
        __FILE__, __LINE__, __FUNCTION__);
    this->OnRequestDefault(object);
  }
}

void ListRequestHandler::OnRequestPush(const common::AsyncRequestObject *object,
    const std::string &element) {
  common::ListResponse response;
  response.request_type_ = common::ListRequestType::LIST_REQUEST_TYPE_PUSH;
  int retcode = common::ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN;
  if(object->storage_type_ & common::StorageType::STORAGE_TYPE_DISK) {
    // 1.Mysql.
    storage::StoragePtr storage = this->storages_.GetStorage(object->key_hash_value_);
    assert(storage);
    if(storage->CheckConnectState() == true) {
      retcode = storage->ListPush(object->key_, element);
      if(retcode == storage::Storage::STORAGE_SERIOUS_ERROR) {
        retcode = common::ListResponseType::LIST_RESPONSE_TYPE_DISCONNECTED;
        // Reconnect mysql.
        RequestServiceSingleton::GetRequestService()->ReconnectStorageServer(storage);
      }
    } else {
      retcode = common::ListResponseType::LIST_RESPONSE_TYPE_DISCONNECTED;
    }
  } else if(object->storage_type_ & common::StorageType::STORAGE_TYPE_CACHE) {
    // 2.Redis.
    redis::RedisPtr redis = this->redises_.GetRedis(object->key_hash_value_);
    assert(redis);
    if(redis->CheckConnectState() == true) {
      retcode = redis->ListPush(object->key_, element);
      if(retcode == redis::Redis::REDIS_SERIOUS_ERROR) {
        retcode = common::ListResponseType::LIST_RESPONSE_TYPE_DISCONNECTED;
        // Reconnect redis.
        RequestServiceSingleton::GetRequestService()->ReconnectRedisServer(redis);
      }
    } else {
      retcode = common::ListResponseType::LIST_RESPONSE_TYPE_DISCONNECTED;
    }
  }
  // 3. Response result.
  response.__set_result_((common::ListResponseType::type)retcode);
  this->async_request_loop_->SendResponse<common::ListResponse>(object->request_id_,
      object->terminal_guid_, object->type_, object->key_, &response);
}

void ListRequestHandler::OnRequestPop(const common::AsyncRequestObject *object) {
  common::ListResponse response;
  response.request_type_ = common::ListRequestType::LIST_REQUEST_TYPE_POP;
  int retcode = common::ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN;
  if(object->storage_type_ & common::StorageType::STORAGE_TYPE_DISK) {
    // 1.Mysql.
    storage::StoragePtr storage = this->storages_.GetStorage(object->key_hash_value_);
    assert(storage);
    if(storage->CheckConnectState() == true) {
      retcode = storage->ListPop(object->key_, response.data_);
      if(retcode == common::ListResponseType::LIST_RESPONSE_TYPE_SUCCESS) {
        response.__isset.data_ = true;
      } else {
        if(retcode == storage::Storage::STORAGE_SERIOUS_ERROR) {
          retcode = common::ListResponseType::LIST_RESPONSE_TYPE_DISCONNECTED;
          // Reconnect mysql.
          RequestServiceSingleton::GetRequestService()->ReconnectStorageServer(storage);
        }
      }
    } else {
      retcode = common::ListResponseType::LIST_RESPONSE_TYPE_DISCONNECTED;
    }
  } else if(object->storage_type_ & common::StorageType::STORAGE_TYPE_CACHE) {
    // 2.Redis.
    redis::RedisPtr redis = this->redises_.GetRedis(object->key_hash_value_);
    assert(redis);
    if(redis->CheckConnectState() == true) {
      retcode = redis->ListPop(object->key_, response.data_);
      if(retcode == common::ListResponseType::LIST_RESPONSE_TYPE_SUCCESS) {
        response.__isset.data_ = true;
      } else {
        if(retcode == redis::Redis::REDIS_SERIOUS_ERROR) {
          retcode = common::ListResponseType::LIST_RESPONSE_TYPE_DISCONNECTED;
          // Reconnect redis.
          RequestServiceSingleton::GetRequestService()->ReconnectRedisServer(redis);
        }
      }
    } else {
      retcode = common::ListResponseType::LIST_RESPONSE_TYPE_DISCONNECTED;
    }
  }
  // 3. Response result.
  response.__set_result_((common::ListResponseType::type)retcode);
  this->async_request_loop_->SendResponse<common::ListResponse>(object->request_id_,
      object->terminal_guid_, object->type_, object->key_, &response);
}

void ListRequestHandler::OnRequestRemove(const common::AsyncRequestObject *object,
    const std::string &element) {
  common::ListResponse response;
  response.request_type_ = common::ListRequestType::LIST_REQUEST_TYPE_REMOVE;
  int retcode = common::ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN;
  if(object->storage_type_ & common::StorageType::STORAGE_TYPE_DISK) {
    // 1.Mysql.
    storage::StoragePtr storage = this->storages_.GetStorage(object->key_hash_value_);
    assert(storage);
    if(storage->CheckConnectState() == true) {
      retcode = storage->ListRemove(object->key_, element);
      if(retcode == storage::Storage::STORAGE_SERIOUS_ERROR) {
        retcode = common::ListResponseType::LIST_RESPONSE_TYPE_DISCONNECTED;
        // Reconnect mysql.
        RequestServiceSingleton::GetRequestService()->ReconnectStorageServer(storage);
      }
    } else {
      retcode = common::ListResponseType::LIST_RESPONSE_TYPE_DISCONNECTED;
    }
  } else if(object->storage_type_ & common::StorageType::STORAGE_TYPE_CACHE) {
    // 2.Redis.
    redis::RedisPtr redis = this->redises_.GetRedis(object->key_hash_value_);
    assert(redis);
    if(redis->CheckConnectState() == true) {
      retcode = redis->ListRemove(object->key_, element);
      if(retcode == redis::Redis::REDIS_SERIOUS_ERROR) {
        retcode = common::ListResponseType::LIST_RESPONSE_TYPE_DISCONNECTED;
        // Reconnect redis.
        RequestServiceSingleton::GetRequestService()->ReconnectRedisServer(redis);
      }
    } else {
      retcode = common::ListResponseType::LIST_RESPONSE_TYPE_DISCONNECTED;
    }
  }
  // 3. Response result.
  response.__set_result_((common::ListResponseType::type)retcode);
  this->async_request_loop_->SendResponse<common::ListResponse>(object->request_id_,
      object->terminal_guid_, object->type_, object->key_, &response);
}

void ListRequestHandler::OnRequestClear(const common::AsyncRequestObject *object) {
  common::ListResponse response;
  response.request_type_ = common::ListRequestType::LIST_REQUEST_TYPE_CLEAR;
  int retcode = common::ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN;
  if(object->storage_type_ & common::StorageType::STORAGE_TYPE_DISK) {
    // 1.Mysql.
    storage::StoragePtr storage = this->storages_.GetStorage(object->key_hash_value_);
    assert(storage);
    if(storage->CheckConnectState() == true) {
      retcode = storage->ListClear(object->key_);
      if(retcode == storage::Storage::STORAGE_SERIOUS_ERROR) {
        retcode = common::ListResponseType::LIST_RESPONSE_TYPE_DISCONNECTED;
        // Reconnect mysql.
        RequestServiceSingleton::GetRequestService()->ReconnectStorageServer(storage);
      }
    } else {
      retcode = common::ListResponseType::LIST_RESPONSE_TYPE_DISCONNECTED;
    }
  } else if(object->storage_type_ & common::StorageType::STORAGE_TYPE_CACHE) {
    // 2.Redis.
    redis::RedisPtr redis = this->redises_.GetRedis(object->key_hash_value_);
    assert(redis);
    if(redis->CheckConnectState() == true) {
      retcode = redis->ListClear(object->key_);
      if(retcode == redis::Redis::REDIS_SERIOUS_ERROR) {
        retcode = common::ListResponseType::LIST_RESPONSE_TYPE_DISCONNECTED;
        // Reconnect redis.
        RequestServiceSingleton::GetRequestService()->ReconnectRedisServer(redis);
      }
    } else {
      retcode = common::ListResponseType::LIST_RESPONSE_TYPE_DISCONNECTED;
    }
  }
  // 3. Response result.
  response.__set_result_((common::ListResponseType::type)retcode);
  this->async_request_loop_->SendResponse<common::ListResponse>(object->request_id_,
      object->terminal_guid_, object->type_, object->key_, &response);
}

void ListRequestHandler::OnRequestDelete(const common::AsyncRequestObject *object) {
  common::ListResponse response;
  response.request_type_ = common::ListRequestType::LIST_REQUEST_TYPE_DELETE;
  int retcode = common::ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN;
  if(object->storage_type_ & common::StorageType::STORAGE_TYPE_DISK) {
    // 1.Mysql.
    storage::StoragePtr storage = this->storages_.GetStorage(object->key_hash_value_);
    assert(storage);
    if(storage->CheckConnectState() == true) {
      retcode = storage->ListDelete(object->key_);
      if(retcode == storage::Storage::STORAGE_SERIOUS_ERROR) {
        retcode = common::ListResponseType::LIST_RESPONSE_TYPE_DISCONNECTED;
        // Reconnect mysql.
        RequestServiceSingleton::GetRequestService()->ReconnectStorageServer(storage);
      }
    } else {
      retcode = common::ListResponseType::LIST_RESPONSE_TYPE_DISCONNECTED;
    }
  } else if(object->storage_type_ & common::StorageType::STORAGE_TYPE_CACHE) {
    // 2.Redis.
    redis::RedisPtr redis = this->redises_.GetRedis(object->key_hash_value_);
    assert(redis);
    if(redis->CheckConnectState() == true) {
      retcode = redis->ListDelete(object->key_);
      if(retcode == redis::Redis::REDIS_SERIOUS_ERROR) {
        retcode = common::ListResponseType::LIST_RESPONSE_TYPE_DISCONNECTED;
        // Reconnect redis.
        RequestServiceSingleton::GetRequestService()->ReconnectRedisServer(redis);
      }
    } else {
      retcode = common::ListResponseType::LIST_RESPONSE_TYPE_DISCONNECTED;
    }
  }
  // 3. Response result.
  response.__set_result_((common::ListResponseType::type)retcode);
  this->async_request_loop_->SendResponse<common::ListResponse>(object->request_id_,
      object->terminal_guid_, object->type_, object->key_, &response);
}

void ListRequestHandler::OnRequestDefault(const common::AsyncRequestObject *object) {
  common::ListResponse response;
  response.__set_result_(common::ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN);
  this->async_request_loop_->SendResponse<common::ListResponse>(object->request_id_,
      object->terminal_guid_, object->type_, object->key_, &response);
}

}  // namespace server

}  // namespace dataproxy

}  // namespace squirrel

