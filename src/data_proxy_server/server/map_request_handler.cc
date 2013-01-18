//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-25 18:15:54.
// File name: map_request_handler.cc
//
// Description: 
// Define class MapRequestHandler.
//

#include "data_proxy_server/server/map_request_handler.h"

#include "common/map_message_types.h"
#include "data_proxy_server/server/async_request_loop.h"
#include "data_proxy_server/server/request_service.h"
#include "global/async_logging_singleton.h"
#include "global/time_consuming.h"

namespace squirrel {

namespace dataproxy {

namespace server {

MapRequestHandler::MapRequestHandler() : async_request_loop_(NULL) {}
MapRequestHandler::~MapRequestHandler() {}

bool MapRequestHandler::Initialize(AsyncRequestLoop *async_request_loop_ptr,
    const RedisConfigure &redis_configure, const StorageConfigure &storage_configure,
    const ProcedureConfigure &procedure_configure) {
  this->async_request_loop_ = async_request_loop_ptr;
  if(this->redises_.Initialize(redis_configure) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Initialize RedisSet failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  if(this->redises_.Start() == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Start RedisSet failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  if(this->storages_.Initialize(storage_configure, procedure_configure) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Initialize StorageSet failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  if(this->storages_.Start() == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Start StorageSet failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  return true;
}

void MapRequestHandler::Request(const common::AsyncRequestObject *object) {
#ifdef _TIME_CONSUMING_TEST
  global::TimeConsuming time_consume("MapRequestHandler::Request");
#endif
  common::MapRequest request;
  if(this->packet_.Deserialize<common::MapRequest>(
        &request, object->request_.data(), object->request_.size()) == true) {
    switch(request.type_) {
      case common::MapRequestType::MAP_REQUEST_TYPE_INSERT:
        this->OnRequestInsert(object, request);
        break;
      case common::MapRequestType::MAP_REQUEST_TYPE_REMOVE:
        this->OnRequestRemove(object, request);
        break;
      case common::MapRequestType::MAP_REQUEST_TYPE_QUERY_RANKING:
        this->OnRequestQueryRanking(object, request);
        break;
      case common::MapRequestType::MAP_REQUEST_TYPE_QUERY_RANKING_RANGE:
        this->OnRequestQueryRange(object, request);
        break;
      case common::MapRequestType::MAP_REQUEST_TYPE_RESET_RANKING_KEY:
        this->OnRequestReset(object, request);
        break;
      case common::MapRequestType::MAP_REQUEST_TYPE_CLEAR:
        this->OnRequestClear(object);
        break;
      case common::MapRequestType::MAP_REQUEST_TYPE_DELETE:
        this->OnRequestDelete(object);
        break;
      default:
        this->OnRequestDefault(object);
        break;
    }
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Deserialize MapRequest failed.",
        __FILE__, __LINE__, __FUNCTION__);
    this->OnRequestDefault(object);
  }
}

void MapRequestHandler::OnRequestInsert(const common::AsyncRequestObject *object,
    const common::MapRequest &request) {
  common::MapResponse response;
  response.type_ = common::MapRequestType::MAP_REQUEST_TYPE_INSERT;
  int retcode = common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
  common::MapInsertRequest insert;
  if(this->packet_.Deserialize<common::MapInsertRequest>(&insert,
        request.data_.data(), request.data_.size()) == true) {
    if(object->storage_type_ & common::StorageType::STORAGE_TYPE_DISK) {
      // 1.Mysql.
      storage::StoragePtr storage = this->storages_.GetStorage(object->key_hash_value_);
      assert(storage);
      if(storage->CheckConnectState() == true) {
        retcode = storage->MapInsert(object->key_, insert.ranking_key_, insert.element_);
        if(retcode == storage::Storage::STORAGE_SERIOUS_ERROR) {
          retcode = common::MapResponseType::MAP_RESPONSE_TYPE_DISCONNECTED;
          // Reconnect mysql.
          RequestServiceSingleton::GetRequestService()->ReconnectStorageServer(storage);
        }
      } else {
        retcode = common::MapResponseType::MAP_RESPONSE_TYPE_DISCONNECTED;
      }
    } else if(object->storage_type_ & common::StorageType::STORAGE_TYPE_CACHE) {
      // 2.Redis.
      redis::RedisPtr redis = this->redises_.GetRedis(object->key_hash_value_);
      assert(redis);
      if(redis->CheckConnectState() == true) {
        retcode = redis->MapInsert(object->key_, insert.ranking_key_, insert.element_);
        if(retcode == redis::Redis::REDIS_SERIOUS_ERROR) {
          retcode = common::MapResponseType::MAP_RESPONSE_TYPE_DISCONNECTED;
          // Reconnect redis.
          RequestServiceSingleton::GetRequestService()->ReconnectRedisServer(redis);
        }
      } else {
        retcode = common::MapResponseType::MAP_RESPONSE_TYPE_DISCONNECTED;
      }
    }
  }
  // 3. Response result.
  response.__set_result_((common::MapResponseType::type)retcode);
  this->async_request_loop_->SendResponse<common::MapResponse>(object->request_id_,
      object->terminal_guid_, object->type_, object->key_, &response);
}
void MapRequestHandler::OnRequestRemove(const common::AsyncRequestObject *object,
    const common::MapRequest &request) {
  common::MapResponse response;
  response.type_ = common::MapRequestType::MAP_REQUEST_TYPE_REMOVE;
  int retcode = common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
  common::MapRemoveRequest remove;
  if(this->packet_.Deserialize<common::MapRemoveRequest>(&remove,
      request.data_.data(), request.data_.size()) == true) {
    if(object->storage_type_ & common::StorageType::STORAGE_TYPE_DISK) {
      // 1.Mysql.
      storage::StoragePtr storage = this->storages_.GetStorage(object->key_hash_value_);
      assert(storage);
      if(storage->CheckConnectState() == true) {
        retcode = storage->MapRemove(object->key_, remove.element_);
        if(retcode == storage::Storage::STORAGE_SERIOUS_ERROR) {
          retcode = common::MapResponseType::MAP_RESPONSE_TYPE_DISCONNECTED;
          // Reconnect mysql.
          RequestServiceSingleton::GetRequestService()->ReconnectStorageServer(storage);
        }
      } else {
        retcode = common::MapResponseType::MAP_RESPONSE_TYPE_DISCONNECTED;
      }
    } else if(object->storage_type_ & common::StorageType::STORAGE_TYPE_CACHE) {
      // 2.Redis.
      redis::RedisPtr redis = this->redises_.GetRedis(object->key_hash_value_);
      assert(redis);
      if(redis->CheckConnectState() == true) {
        retcode = redis->MapRemove(object->key_, remove.element_);
        if(retcode == redis::Redis::REDIS_SERIOUS_ERROR) {
          retcode = common::MapResponseType::MAP_RESPONSE_TYPE_DISCONNECTED;
          // Reconnect redis.
          RequestServiceSingleton::GetRequestService()->ReconnectRedisServer(redis);
        }
      } else {
        retcode = common::MapResponseType::MAP_RESPONSE_TYPE_DISCONNECTED;
      }
    }
  }
  // 3. Response result.
  response.__set_result_((common::MapResponseType::type)retcode);
  this->async_request_loop_->SendResponse<common::MapResponse>(object->request_id_,
      object->terminal_guid_, object->type_, object->key_, &response);
}

void MapRequestHandler::OnRequestQueryRanking(const common::AsyncRequestObject *object,
    const common::MapRequest &request) {
  common::MapResponse response;
  response.type_ = common::MapRequestType::MAP_REQUEST_TYPE_QUERY_RANKING;
  int retcode = common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
  common::MapQueryRankingRequest query;
  if(this->packet_.Deserialize<common::MapQueryRankingRequest>(&query,
        request.data_.data(), request.data_.size()) == true) {
    if(object->storage_type_ & common::StorageType::STORAGE_TYPE_DISK) {
      // 1.Mysql.
      storage::StoragePtr storage = this->storages_.GetStorage(object->key_hash_value_);
      assert(storage);
      if(storage->CheckConnectState() == true) {
        common::MapQueryRankingResponse ranking;
        retcode = storage->MapQueryRank(object->key_, query.element_, ranking.ranking_,
            query.sort_type_ == common::MapSortType::MAP_SORT_TYPE_ASC);
        if(retcode == common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS) {
          size_t size = 0;
          const char *ptr = this->packet_.Serialize<common::MapQueryRankingResponse>(&ranking, size);
          if(ptr) {
            response.__isset.data_ = true;
            response.data_.assign(ptr, size);
          } else {
            retcode = common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
          }
        }
        if(retcode == storage::Storage::STORAGE_SERIOUS_ERROR) {
          retcode = common::MapResponseType::MAP_RESPONSE_TYPE_DISCONNECTED;
          // Reconnect mysql.
          RequestServiceSingleton::GetRequestService()->ReconnectStorageServer(storage);
        }
      } else {
        retcode = common::MapResponseType::MAP_RESPONSE_TYPE_DISCONNECTED;
      }
    } else if(object->storage_type_ & common::StorageType::STORAGE_TYPE_CACHE) {
      // 2.Redis.
      redis::RedisPtr redis = this->redises_.GetRedis(object->key_hash_value_);
      assert(redis);
      if(redis->CheckConnectState() == true) {
        common::MapQueryRankingResponse ranking;
        retcode = redis->MapQueryRank(object->key_, query.element_, ranking.ranking_,
            query.sort_type_ == common::MapSortType::MAP_SORT_TYPE_ASC);
        if(retcode == common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS) {
          size_t size = 0;
          const char *ptr = this->packet_.Serialize<common::MapQueryRankingResponse>(&ranking, size);
          if(ptr) {
            response.__isset.data_ = true;
            response.data_.assign(ptr, size);
          } else {
            retcode = common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
          }
        }
        if(retcode == redis::Redis::REDIS_SERIOUS_ERROR) {
          retcode = common::MapResponseType::MAP_RESPONSE_TYPE_DISCONNECTED;
          // Reconnect mysql.
          RequestServiceSingleton::GetRequestService()->ReconnectRedisServer(redis);
        }
        if(retcode == redis::Redis::REDIS_SERIOUS_ERROR) {
          retcode = common::MapResponseType::MAP_RESPONSE_TYPE_DISCONNECTED;
          // Reconnect redis.
          RequestServiceSingleton::GetRequestService()->ReconnectRedisServer(redis);
        }
      } else {
        retcode = common::MapResponseType::MAP_RESPONSE_TYPE_DISCONNECTED;
      }
    }
  }
  // 3. Response result.
  response.__set_result_((common::MapResponseType::type)retcode);
  this->async_request_loop_->SendResponse<common::MapResponse>(object->request_id_,
      object->terminal_guid_, object->type_, object->key_, &response);
}

void MapRequestHandler::OnRequestQueryRange(const common::AsyncRequestObject *object,
    const common::MapRequest &request) {
  common::MapResponse response;
  response.type_ = common::MapRequestType::MAP_REQUEST_TYPE_QUERY_RANKING_RANGE;
  int retcode = common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
  common::MapQueryRankingRangeRequest query;
  if(this->packet_.Deserialize<common::MapQueryRankingRangeRequest>(&query,
      request.data_.data(), request.data_.size()) == true) {
    if(object->storage_type_ & common::StorageType::STORAGE_TYPE_DISK) {
      // 1.Mysql.
      storage::StoragePtr storage = this->storages_.GetStorage(object->key_hash_value_);
      assert(storage);
      if(storage->CheckConnectState() == true) {
        retcode = storage->MapQueryRangeWithscores(object->key_, query.start_, query.end_,
            query.sort_type_ == common::MapSortType::MAP_SORT_TYPE_ASC);
        if(retcode == common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS) {
          common::MapQueryRankingRangeResponse range;
          common::MapQueryRankingRangeInfo row;
          int index = 0;
          char buff[20];
          while(storage->MapFetchRangeValueWithscores(row.ranking_key_,
                row.elements_, index) == true) {
            snprintf(buff, sizeof(buff), "%d", index++);
            row.ranking_ = buff;
            range.elements_.push_back(row);
          }
          size_t size = 0;
          const char *ptr = this->packet_.Serialize<common::MapQueryRankingRangeResponse>(&range, size);
          if(ptr) {
            response.__isset.data_ = true;
            response.data_.assign(ptr, size);
          } else {
            retcode = common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
          }
        }
        if(retcode == storage::Storage::STORAGE_SERIOUS_ERROR) {
          retcode = common::MapResponseType::MAP_RESPONSE_TYPE_DISCONNECTED;
          // Reconnect mysql.
          RequestServiceSingleton::GetRequestService()->ReconnectStorageServer(storage);
        }
      } else {
        retcode = common::MapResponseType::MAP_RESPONSE_TYPE_DISCONNECTED;
      }
    } else if(object->storage_type_ & common::StorageType::STORAGE_TYPE_CACHE) {
      // 2.Redis.
      redis::RedisPtr redis = this->redises_.GetRedis(object->key_hash_value_);
      assert(redis);
      if(redis->CheckConnectState() == true) {
        retcode = redis->MapQueryRangeWithscores(object->key_, query.start_, query.end_,
            query.sort_type_ == common::MapSortType::MAP_SORT_TYPE_ASC);
        if(retcode == common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS) {
          common::MapQueryRankingRangeResponse range;
          common::MapQueryRankingRangeInfo row;
          int index = 0;
          char buff[20];
          while(redis->MapFetchRangeValueWithscores(row.ranking_key_,
                row.elements_, index) == true) {
            snprintf(buff, sizeof(buff), "%d", index++);
            row.ranking_ = buff;
            range.elements_.push_back(row);
          }
          size_t size = 0;
          const char *ptr = this->packet_.Serialize<common::MapQueryRankingRangeResponse>(&range, size);
          if(ptr) {
            response.__isset.data_ = true;
            response.data_.assign(ptr, size);
          } else {
            retcode = common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
          }
        }
        if(retcode == redis::Redis::REDIS_SERIOUS_ERROR) {
          retcode = common::MapResponseType::MAP_RESPONSE_TYPE_DISCONNECTED;
          // Reconnect mysql.
          RequestServiceSingleton::GetRequestService()->ReconnectRedisServer(redis);
        }
        if(retcode == redis::Redis::REDIS_SERIOUS_ERROR) {
          retcode = common::MapResponseType::MAP_RESPONSE_TYPE_DISCONNECTED;
          // Reconnect redis.
          RequestServiceSingleton::GetRequestService()->ReconnectRedisServer(redis);
        }
      } else {
        retcode = common::MapResponseType::MAP_RESPONSE_TYPE_DISCONNECTED;
      }
    }
  }
  // 3. Response result.
  response.__set_result_((common::MapResponseType::type)retcode);
  this->async_request_loop_->SendResponse<common::MapResponse>(object->request_id_,
      object->terminal_guid_, object->type_, object->key_, &response);
}

void MapRequestHandler::OnRequestReset(const common::AsyncRequestObject *object,
    const common::MapRequest &request) {
  common::MapResponse response;
  response.type_ = common::MapRequestType::MAP_REQUEST_TYPE_RESET_RANKING_KEY;
  int retcode = common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
  common::MapResetRankingKeyRequest reset;
  if(this->packet_.Deserialize<common::MapResetRankingKeyRequest>(&reset,
      request.data_.data(), request.data_.size()) == true) {
    if(object->storage_type_ & common::StorageType::STORAGE_TYPE_DISK) {
      // 1.Mysql.
      storage::StoragePtr storage = this->storages_.GetStorage(object->key_hash_value_);
      assert(storage);
      if(storage->CheckConnectState() == true) {
        retcode = storage->MapResetScore(object->key_, reset.ranking_key_, reset.element_);
        if(retcode == storage::Storage::STORAGE_SERIOUS_ERROR) {
          retcode = common::MapResponseType::MAP_RESPONSE_TYPE_DISCONNECTED;
          // Reconnect mysql.
          RequestServiceSingleton::GetRequestService()->ReconnectStorageServer(storage);
        }
      } else {
        retcode = common::MapResponseType::MAP_RESPONSE_TYPE_DISCONNECTED;
      }
    } else if(object->storage_type_ & common::StorageType::STORAGE_TYPE_CACHE) {
      // 2.Redis.
      redis::RedisPtr redis = this->redises_.GetRedis(object->key_hash_value_);
      assert(redis);
      if(redis->CheckConnectState() == true) {
        retcode = redis->MapResetScore(object->key_, reset.ranking_key_, reset.element_);
        if(retcode == redis::Redis::REDIS_SERIOUS_ERROR) {
          retcode = common::MapResponseType::MAP_RESPONSE_TYPE_DISCONNECTED;
          // Reconnect redis.
          RequestServiceSingleton::GetRequestService()->ReconnectRedisServer(redis);
        }
      } else {
        retcode = common::MapResponseType::MAP_RESPONSE_TYPE_DISCONNECTED;
      }
    }
  }
  // 3. Response result.
  response.__set_result_((common::MapResponseType::type)retcode);
  this->async_request_loop_->SendResponse<common::MapResponse>(object->request_id_,
      object->terminal_guid_, object->type_, object->key_, &response);
}

void MapRequestHandler::OnRequestClear(const common::AsyncRequestObject *object) {
  common::MapResponse response;
  response.type_ = common::MapRequestType::MAP_REQUEST_TYPE_CLEAR;
  int retcode = common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
  if(object->storage_type_ & common::StorageType::STORAGE_TYPE_DISK) {
    // 1.Mysql.
    storage::StoragePtr storage = this->storages_.GetStorage(object->key_hash_value_);
    assert(storage);
    if(storage->CheckConnectState() == true) {
      retcode = storage->MapClear(object->key_);
      if(retcode == storage::Storage::STORAGE_SERIOUS_ERROR) {
        retcode = common::MapResponseType::MAP_RESPONSE_TYPE_DISCONNECTED;
        // Reconnect mysql.
        RequestServiceSingleton::GetRequestService()->ReconnectStorageServer(storage);
      }
    } else {
      retcode = common::MapResponseType::MAP_RESPONSE_TYPE_DISCONNECTED;
    }
  } else if(object->storage_type_ & common::StorageType::STORAGE_TYPE_CACHE) {
    // 2.Redis.
    redis::RedisPtr redis = this->redises_.GetRedis(object->key_hash_value_);
    assert(redis);
    if(redis->CheckConnectState() == true) {
      retcode = redis->MapClear(object->key_);
      if(retcode == redis::Redis::REDIS_SERIOUS_ERROR) {
        retcode = common::MapResponseType::MAP_RESPONSE_TYPE_DISCONNECTED;
        // Reconnect redis.
        RequestServiceSingleton::GetRequestService()->ReconnectRedisServer(redis);
      }
    } else {
      retcode = common::MapResponseType::MAP_RESPONSE_TYPE_DISCONNECTED;
    }
  }
  // 3. Response result.
  response.__set_result_((common::MapResponseType::type)retcode);
  this->async_request_loop_->SendResponse<common::MapResponse>(object->request_id_,
      object->terminal_guid_, object->type_, object->key_, &response);
}

void MapRequestHandler::OnRequestDelete(const common::AsyncRequestObject *object) {
  common::MapResponse response;
  response.type_ = common::MapRequestType::MAP_REQUEST_TYPE_DELETE;
  int retcode = common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
  if(object->storage_type_ & common::StorageType::STORAGE_TYPE_DISK) {
    // 1.Mysql.
    storage::StoragePtr storage = this->storages_.GetStorage(object->key_hash_value_);
    assert(storage);
    if(storage->CheckConnectState() == true) {
      retcode = storage->MapDelete(object->key_);
      if(retcode == storage::Storage::STORAGE_SERIOUS_ERROR) {
        retcode = common::MapResponseType::MAP_RESPONSE_TYPE_DISCONNECTED;
        // Reconnect mysql.
        RequestServiceSingleton::GetRequestService()->ReconnectStorageServer(storage);
      }
    } else {
      retcode = common::MapResponseType::MAP_RESPONSE_TYPE_DISCONNECTED;
    }
  } else if(object->storage_type_ & common::StorageType::STORAGE_TYPE_CACHE) {
    // 2.Redis.
    redis::RedisPtr redis = this->redises_.GetRedis(object->key_hash_value_);
    assert(redis);
    if(redis->CheckConnectState() == true) {
      retcode = redis->MapDelete(object->key_);
      if(retcode == redis::Redis::REDIS_SERIOUS_ERROR) {
        retcode = common::MapResponseType::MAP_RESPONSE_TYPE_DISCONNECTED;
        // Reconnect redis.
        RequestServiceSingleton::GetRequestService()->ReconnectRedisServer(redis);
      }
    } else {
      retcode = common::MapResponseType::MAP_RESPONSE_TYPE_DISCONNECTED;
    }
  }
  // 3. Response result.
  response.__set_result_((common::MapResponseType::type)retcode);
  this->async_request_loop_->SendResponse<common::MapResponse>(object->request_id_,
      object->terminal_guid_, object->type_, object->key_, &response);
}

void MapRequestHandler::OnRequestDefault(const common::AsyncRequestObject *object) {
  common::MapResponse response;
  response.__set_result_(common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN);
  this->async_request_loop_->SendResponse<common::MapResponse>(object->request_id_,
      object->terminal_guid_, object->type_, object->key_, &response);
}

}  // namespace server

}  // namespace dataproxy

}  // namespace squirrel

