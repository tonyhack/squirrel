//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-19 18:04:01.
// File name: number_request_handler.cc
//
// Description: 
// Define class NumberRequestHandler.
//

#include "data_proxy_server/server/number_request_handler.h"

#include "common/number_message_types.h"
#include "data_proxy_server/server/async_request_loop.h"
#include "data_proxy_server/server/request_service.h"
#include "global/async_logging_singleton.h"
#include "global/time_consuming.h"

namespace squirrel {

namespace dataproxy {

namespace server {

NumberRequestHandler::NumberRequestHandler() {}

NumberRequestHandler::~NumberRequestHandler() {}

bool NumberRequestHandler::Initialize(AsyncRequestLoop *async_request_loop_ptr,
    const RedisConfigure &redis_configure, const StorageConfigure &storage_configure,
    const ProcedureConfigure &procedure_configure) {
  this->async_request_loop_ = async_request_loop_ptr;
  if(this->redises_.Initialize(redis_configure) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [NumberRequestHandler] Initialize RedisSet failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  if(this->redises_.Start() == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [NumberRequestHandler] Start RedisSet failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  if(this->storages_.Initialize(storage_configure, procedure_configure) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [NumberRequestHandler] Initialize StorageSet failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  if(this->storages_.Start() == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [NumberRequestHandler] Start StorageSet failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  return true;
}

void NumberRequestHandler::Request(const common::AsyncRequestObject *object) {
#ifdef _TIME_CONSUMING_TEST
  global::TimeConsuming time_consume("NumberRequestHandler::Request");
#endif
  common::NumberRequest request;
  if(this->packet_.Deserialize<common::NumberRequest>(
        &request, object->request_.data(), object->request_.size()) == true) {
    switch(request.type_) {
      case common::NumberRequestType::NUMBER_REQUEST_TYPE_INC:
        this->OnRequestIncrease(object, request.increment_);
        break;
      case common::NumberRequestType::NUMBER_REQUEST_TYPE_DEC:
        this->OnRequestDecrease(object, request.increment_);
        break;
      case common::NumberRequestType::NUMBER_REQUEST_TYPE_RESET:
        this->OnRequestReset(object);
        break;
      case common::NumberRequestType::NUMBER_REQUEST_TYPE_DELETE:
        this->OnRequestDelete(object);
        break;
      default:
        this->OnRequestDefault(object);
        break;
    }
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [NumberRequestHandler] Deserialize NumberRequest failed.",
        __FILE__, __LINE__, __FUNCTION__);
    this->OnRequestDefault(object);
  }
}

void NumberRequestHandler::OnRequestIncrease(const common::AsyncRequestObject *object,
  const std::string &increment) {
  common::NumberResponse response;
  response.type_ = common::NumberRequestType::NUMBER_REQUEST_TYPE_INC;
  int retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_UNKNOWN;
  if(object->storage_type_ & common::StorageType::STORAGE_TYPE_DISK) {
    // 1.Mysql.
    storage::StoragePtr storage = this->storages_.GetStorage(object->key_hash_value_);
    assert(storage);
    if(storage->CheckConnectState() == true) {
      retcode = storage->NumberIncrease(object->key_, increment, response.value_);
      if(retcode == common::NumberResponseType::NUMBER_RESPONSE_TYPE_SUCCESS) {
        response.__isset.value_ = true;
      } else {
        if(retcode == storage::Storage::STORAGE_SERIOUS_ERROR) {
          retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_DISCONNECTED;
          // Reconnect mysql.
          RequestServiceSingleton::GetRequestService()->ReconnectStorageServer(storage);
        }
      }
    } else {
      retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_DISCONNECTED;
    }
  } else if(object->storage_type_ & common::StorageType::STORAGE_TYPE_CACHE) {
    // 2.Redis.
    redis::RedisPtr redis = this->redises_.GetRedis(object->key_hash_value_);
    assert(redis);
    if(redis->CheckConnectState() == true) {
      retcode = redis->NumberIncrease(object->key_, increment, response.value_);
      if(retcode == common::NumberResponseType::NUMBER_RESPONSE_TYPE_SUCCESS) {
        response.__isset.value_ = true;
      } else {
        // Get failed from redis.
        if(retcode == redis::Redis::REDIS_SERIOUS_ERROR) {
          retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_DISCONNECTED;
          // Reconnect redis.
          RequestServiceSingleton::GetRequestService()->ReconnectRedisServer(redis);
        }
      }
    } else {
      retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_DISCONNECTED;
    }
  }
  // 3. Response result.
  response.__set_result_((common::NumberResponseType::type)retcode);
  this->async_request_loop_->SendResponse<common::NumberResponse>(object->request_id_,
      object->terminal_guid_, object->type_, object->key_, &response);
}

void NumberRequestHandler::OnRequestDecrease(const common::AsyncRequestObject *object,
    const std::string &decrement) {
  common::NumberResponse response;
  response.type_ = common::NumberRequestType::NUMBER_REQUEST_TYPE_DEC;
  int retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_UNKNOWN;
  if(object->storage_type_ & common::StorageType::STORAGE_TYPE_DISK) {
    // 1.Mysql.
    storage::StoragePtr storage = this->storages_.GetStorage(object->key_hash_value_);
    assert(storage);
    if(storage->CheckConnectState() == true) {
      retcode = storage->NumberDecrease(object->key_, decrement, response.value_);
      if(retcode == common::NumberResponseType::NUMBER_RESPONSE_TYPE_SUCCESS) {
        response.__isset.value_ = true;
      } else {
        if(retcode == storage::Storage::STORAGE_SERIOUS_ERROR) {
          retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_DISCONNECTED;
          // Reconnect mysql.
          RequestServiceSingleton::GetRequestService()->ReconnectStorageServer(storage);
        }
      }
    } else {
      retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_DISCONNECTED;
    }
  } else if(object->storage_type_ & common::StorageType::STORAGE_TYPE_CACHE) {
    // 2.Redis.
    redis::RedisPtr redis = this->redises_.GetRedis(object->key_hash_value_);
    assert(redis);
    if(redis->CheckConnectState() == true) {
      retcode = redis->NumberDecrease(object->key_, decrement, response.value_);
      if(retcode == common::NumberResponseType::NUMBER_RESPONSE_TYPE_SUCCESS) {
        response.__isset.value_ = true;
      } else {
        // Get failed from redis.
        if(retcode == redis::Redis::REDIS_SERIOUS_ERROR) {
          retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_DISCONNECTED;
          // Reconnect redis.
          RequestServiceSingleton::GetRequestService()->ReconnectRedisServer(redis);
        }
      }
    } else {
      retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_DISCONNECTED;
    }
  }
  // 3. Response result.
  response.__set_result_((common::NumberResponseType::type)retcode);
  this->async_request_loop_->SendResponse<common::NumberResponse>(object->request_id_,
      object->terminal_guid_, object->type_, object->key_, &response);
}

void NumberRequestHandler::OnRequestReset(const common::AsyncRequestObject *object) {
  common::NumberResponse response;
  response.type_ = common::NumberRequestType::NUMBER_REQUEST_TYPE_RESET;
  int retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_UNKNOWN;
  if(object->storage_type_ & common::StorageType::STORAGE_TYPE_DISK) {
    // 1.Mysql.
    storage::StoragePtr storage = this->storages_.GetStorage(object->key_hash_value_);
    assert(storage);
    if(storage->CheckConnectState() == true) {
      retcode = storage->NumberReset(object->key_);
      if(retcode == storage::Storage::STORAGE_SERIOUS_ERROR) {
        retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_DISCONNECTED;
        // Reconnect mysql.
        RequestServiceSingleton::GetRequestService()->ReconnectStorageServer(storage);
      }
    } else {
      retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_DISCONNECTED;
    }
  } else if(object->storage_type_ & common::StorageType::STORAGE_TYPE_CACHE) {
    // 2.Redis.
    redis::RedisPtr redis = this->redises_.GetRedis(object->key_hash_value_);
    assert(redis);
    if(redis->CheckConnectState() == true) {
      retcode = redis->NumberReset(object->key_);
      // Get failed from redis.
      if(retcode == redis::Redis::REDIS_SERIOUS_ERROR) {
        retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_DISCONNECTED;
        // Reconnect redis.
        RequestServiceSingleton::GetRequestService()->ReconnectRedisServer(redis);
      }
    } else {
      retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_DISCONNECTED;
    }
  }
  // 3. Response result.
  response.__set_result_((common::NumberResponseType::type)retcode);
  this->async_request_loop_->SendResponse<common::NumberResponse>(object->request_id_,
      object->terminal_guid_, object->type_, object->key_, &response);
}

void NumberRequestHandler::OnRequestDelete(const common::AsyncRequestObject *object) {
  common::NumberResponse response;
  response.type_ = common::NumberRequestType::NUMBER_REQUEST_TYPE_DELETE;
  int retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_UNKNOWN;
  if(object->storage_type_ & common::StorageType::STORAGE_TYPE_DISK) {
    // 1.Mysql.
    storage::StoragePtr storage = this->storages_.GetStorage(object->key_hash_value_);
    assert(storage);
    if(storage->CheckConnectState() == true) {
      retcode = storage->NumberDelete(object->key_);
      if(retcode == storage::Storage::STORAGE_SERIOUS_ERROR) {
        retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_DISCONNECTED;
        // Reconnect mysql.
        RequestServiceSingleton::GetRequestService()->ReconnectStorageServer(storage);
      }
    } else {
      retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_DISCONNECTED;
    }
  } else if(object->storage_type_ & common::StorageType::STORAGE_TYPE_CACHE) {
    // 2.Redis.
    redis::RedisPtr redis = this->redises_.GetRedis(object->key_hash_value_);
    assert(redis);
    if(redis->CheckConnectState() == true) {
      retcode = redis->NumberDelete(object->key_);
      // Get failed from redis.
      if(retcode == redis::Redis::REDIS_SERIOUS_ERROR) {
        retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_DISCONNECTED;
        // Reconnect redis.
        RequestServiceSingleton::GetRequestService()->ReconnectRedisServer(redis);
      }
    } else {
      retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_DISCONNECTED;
    }
  }
  // 3. Response result.
  response.__set_result_((common::NumberResponseType::type)retcode);
  this->async_request_loop_->SendResponse<common::NumberResponse>(object->request_id_,
      object->terminal_guid_, object->type_, object->key_, &response);
}

void NumberRequestHandler::OnRequestDefault(const common::AsyncRequestObject *object) {
  common::NumberResponse response;
  response.__set_result_(common::NumberResponseType::NUMBER_RESPONSE_TYPE_UNKNOWN);
  this->async_request_loop_->SendResponse<common::NumberResponse>(object->request_id_,
      object->terminal_guid_, object->type_, object->key_, &response);
}

}  // namespace server

}  // namespace dataproxy

}  // namespace squirrel

