//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-04 20:50:43.
// File name: redis.cc
//
// Description: 
// Define class Redis.
//

#include "data_proxy_server/redis_proxy/redis.h"

#include <sstream>

#include "common/keyvalue_message_types.h"
#include "common/number_message_types.h"
#include "common/list_message_types.h"
#include "common/map_message_types.h"
#include "global/async_logging_singleton.h"

#ifdef _REDIS_MONITOR_
#include "core/base/time_tick.h"
#endif

namespace squirrel {

using namespace global;

namespace dataproxy {

namespace redis {

Redis::Redis() : connecting_(false), context_(NULL), reply_(NULL) {}
Redis::~Redis() {
  if(this->context_) {
    this->connecting_ = false;
    redisFree(this->context_);
  }
}

bool Redis::Initialize(const std::string &host, core::uint16 port) {
  this->host_ = host;
  this->port_ = port;
  return true;
}

bool Redis::Start() {
  struct timeval timeout = { 1, 500000 }; // 1.5 seconds
  this->context_ = redisConnectWithTimeout(this->host_.c_str(), this->port_, timeout);
  if(this->context_->err) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Redis] Connect redis[%s,%d] server error[%d].",
        __FILE__, __LINE__, __FUNCTION__, this->host_.c_str(), this->port_, this->context_->err);
    return false;
  }
  this->connecting_ = true;
  return true;
}

bool Redis::Reconnect() {
  if(this->CheckConnectState() == false) {
    if(this->context_) {
      redisFree(this->context_);
      this->context_ = NULL;
    }

    struct timeval timeout = { 1, 500000 }; // 1.5 seconds
    this->context_ = redisConnectWithTimeout(this->host_.c_str(), this->port_, timeout);
    if(this->context_->err) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] Reconnect redis[%s,%d] server error[%d].",
          __FILE__, __LINE__, __FUNCTION__, this->host_.c_str(),
          this->port_, this->context_->err);
      return false;
    }

    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
        "[Redis] Reconnect redis server ok.");

    this->connecting_ = true;
  }
  return true;
}

int Redis::KeyvalueSet(const std::string &key, const std::string &value) {
#ifdef _REDIS_MONITOR_
  core::TimeTick now;
  now.Update();
  core::TimestampMillisecond timestamp1(now);
#endif
  this->reply_ = (redisReply *)redisCommand(this->context_, "SET %b %b",
      key.data(), key.size(), value.data(), value.size());
#ifdef _REDIS_MONITOR_
  now.Update();
  core::TimestampMillisecond timestamp2(now);
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
      "[_REDIS_MONITOR_] redisCommand set cast [%ld] ms.", timestamp2.AbsoluteDistance(timestamp1));
#endif
  if(this->reply_ == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Redis] redisCommand occur serious error[%d].",
        __FILE__, __LINE__, __FUNCTION__, this->context_->err);
    this->connecting_ = false;
    redisFree(this->context_);
    this->context_ = NULL;
    return REDIS_SERIOUS_ERROR;
  } else {
    int retcode = 0;
    if(this->reply_->type == REDIS_REPLY_STATUS && strcasecmp(this->reply_->str,"ok") == 0) {
      retcode = common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_SUCCESS;
    } else if(this->reply_->type == REDIS_REPLY_ERROR) {
      retcode = common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] keyvalue request error [%s].",
          __FILE__, __LINE__, __FUNCTION__, this->reply_->str);
    } else {
      retcode = common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] keyvalue request unknown error.",
          __FILE__, __LINE__, __FUNCTION__);
    }
    freeReplyObject(this->reply_);
    return retcode;
  }
}

int Redis::KeyvalueGet(const std::string &key, std::string &value) {
  this->reply_ = (redisReply *)redisCommand(this->context_,
      "GET %b", key.data(), key.size());
  if(this->reply_ == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Redis] redisCommand occur serious error[%d].",
        __FILE__, __LINE__, __FUNCTION__, this->context_->err);
    this->connecting_ = false;
    redisFree(this->context_);
    this->context_ = NULL;
    return REDIS_SERIOUS_ERROR;
  } else {
    int retcode = 0;
    if(this->reply_->type == REDIS_REPLY_STRING) {
      value.assign(this->reply_->str, this->reply_->len);
      retcode = common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_SUCCESS;
    } else if(this->reply_->type == REDIS_REPLY_NIL) {
      retcode = common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_KEY_NOT_EXIST;
    } else if(this->reply_->type == REDIS_REPLY_ERROR) {
      retcode = common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] keyvalue request error [%s].",
          __FILE__, __LINE__, __FUNCTION__, this->reply_->str);
    } else {
      retcode = common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] keyvalue request unknown error.",
          __FILE__, __LINE__, __FUNCTION__);
    }
    freeReplyObject(this->reply_);
    return retcode;
  }
}

int Redis::KeyvalueDelete(const std::string &key) {
  this->reply_ = (redisReply *)redisCommand(this->context_,
      "DEL %b", key.data(), key.size());
  if(this->reply_ == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Redis] redisCommand occur serious error[%d].",
        __FILE__, __LINE__, __FUNCTION__, this->context_->err);
    this->connecting_ = false;
    redisFree(this->context_);
    this->context_ = NULL;
    return REDIS_SERIOUS_ERROR;
  } else {
    int retcode = 0;
    if(this->reply_->type == REDIS_REPLY_INTEGER) {
      retcode = common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_SUCCESS;
    /*
      if(this->reply_->integer > 0) {
        retcode = common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_SUCCESS;
      } else {
        retcode = common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_KEY_NOT_EXIST;
      }
      */
    } else if(this->reply_->type == REDIS_REPLY_NIL) {
      // retcode = common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_KEY_NOT_EXIST;
      retcode = common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_SUCCESS;
    } else if(this->reply_->type == REDIS_REPLY_ERROR) {
      retcode = common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] keyvalue request error [%s].",
          __FILE__, __LINE__, __FUNCTION__, this->reply_->str);
    } else {          
      retcode = common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] keyvalue request unknown error.",
          __FILE__, __LINE__, __FUNCTION__);
    }
    freeReplyObject(this->reply_);
    return retcode;
  }
}

int Redis::NumberIncrease(const std::string &key,
    const std::string &increment, std::string &value) {
  this->reply_ = (redisReply *)redisCommand(this->context_,
      "INCRBY %b %s", key.data(), key.size(), increment.c_str());
  if(this->reply_ == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Redis] redisCommand occur serious error[%d].",
        __FILE__, __LINE__, __FUNCTION__, this->context_->err);
    this->connecting_ = false;
    redisFree(this->context_);
    this->context_ = NULL;
    return REDIS_SERIOUS_ERROR;
  } else {
    int retcode = 0;
    if(this->reply_->type == REDIS_REPLY_INTEGER) {
      std::ostringstream oss;
      oss << this->reply_->integer;
      value = oss.str();
      retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_SUCCESS;
    } else if(this->reply_->type == REDIS_REPLY_ERROR) {
      retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] number request error [%s].",
          __FILE__, __LINE__, __FUNCTION__, this->reply_->str);
    } else {              
      retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] number request unknown error.",
          __FILE__, __LINE__, __FUNCTION__);
    }
    freeReplyObject(this->reply_);
    return retcode;
  }
}

int Redis::NumberDecrease(const std::string &key,
    const std::string &decrement, std::string &value) {
  this->reply_ = (redisReply *)redisCommand(this->context_,
      "DECRBY %b %s", key.data(), key.size(), decrement.c_str());
  if(this->reply_ == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Redis] redisCommand occur serious error[%d].",
        __FILE__, __LINE__, __FUNCTION__, this->context_->err);
    this->connecting_ = false;
    redisFree(this->context_);
    this->context_ = NULL;
    return REDIS_SERIOUS_ERROR;
  } else {
    int retcode = 0;
    if(this->reply_->type == REDIS_REPLY_INTEGER) {
      std::ostringstream oss;
      oss << this->reply_->integer;
      value = oss.str();
      retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_SUCCESS;
    } else if(this->reply_->type == REDIS_REPLY_ERROR) {
      retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] number request error [%s].",
          __FILE__, __LINE__, __FUNCTION__, this->reply_->str);
    } else {              
      retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] number request unknown error.",
          __FILE__, __LINE__, __FUNCTION__);
    }
    freeReplyObject(this->reply_);
    return retcode;
  }
}

int Redis::NumberReset(const std::string &key) {
  this->reply_ = (redisReply *)redisCommand(this->context_, "SET %b 0",
      key.data(), key.size());
  if(this->reply_ == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Redis] redisCommand occur serious error[%d].",
        __FILE__, __LINE__, __FUNCTION__, this->context_->err);
    this->connecting_ = false;
    redisFree(this->context_);
    this->context_ = NULL;
    return REDIS_SERIOUS_ERROR;
  } else {
    int retcode = 0;
    if(this->reply_->type == REDIS_REPLY_STATUS && strcasecmp(this->reply_->str,"ok") == 0) {
      retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_SUCCESS;
    } else if(this->reply_->type == REDIS_REPLY_ERROR) {
      retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] number request error [%s].",
          __FILE__, __LINE__, __FUNCTION__, this->reply_->str);
    } else {
      retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] number request unknown error.",
          __FILE__, __LINE__, __FUNCTION__);
    }
    freeReplyObject(this->reply_);
    return retcode;
  }
}

int Redis::NumberDelete(const std::string &key) {
  this->reply_ = (redisReply *)redisCommand(this->context_,
      "DEL %b", key.data(), key.size());
  if(this->reply_ == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Redis] redisCommand occur serious error[%d].",
        __FILE__, __LINE__, __FUNCTION__, this->context_->err);
    this->connecting_ = false;
    redisFree(this->context_);
    this->context_ = NULL;
    return REDIS_SERIOUS_ERROR;
  } else {
    int retcode = 0;
    if(this->reply_->type == REDIS_REPLY_INTEGER) {
      if(this->reply_->integer > 0) {
        retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_SUCCESS;
      } else {
        retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_KEY_NOT_EXIST;
      }
    } else if(this->reply_->type == REDIS_REPLY_NIL) {
      retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_KEY_NOT_EXIST;
    } else if(this->reply_->type == REDIS_REPLY_ERROR) {
      retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] number request error [%s].",
          __FILE__, __LINE__, __FUNCTION__, this->reply_->str);
    } else {          
      retcode = common::NumberResponseType::NUMBER_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] number request unknown error.",
          __FILE__, __LINE__, __FUNCTION__);
    }
    freeReplyObject(this->reply_);
    return retcode;
  }
}

int Redis::ListPush(const std::string &key, const std::string &element) {
  this->reply_ = (redisReply *)redisCommand(this->context_,
      "SADD %b %b", key.data(), key.size(), element.data(), element.size());
  if(this->reply_ == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Redis] redisCommand occur serious error[%d].",
        __FILE__, __LINE__, __FUNCTION__, this->context_->err);
    this->connecting_ = false;
    redisFree(this->context_);
    this->context_ = NULL;
    return REDIS_SERIOUS_ERROR;
  } else {
    int retcode = 0;
    if(this->reply_->type == REDIS_REPLY_INTEGER) {
      if(this->reply_->integer > 0) {
        retcode = common::ListResponseType::LIST_RESPONSE_TYPE_SUCCESS;
      } else {
        retcode = common::ListResponseType::LIST_RESPONSE_TYPE_ELEM_ALREDY_EXIST;
      }
    } else if(this->reply_->type == REDIS_REPLY_ERROR) {
      retcode = common::ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] list request error [%s].",
          __FILE__, __LINE__, __FUNCTION__, this->reply_->str);
    } else {
      retcode = common::ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] list request unknown error.",
          __FILE__, __LINE__, __FUNCTION__);
    }
    freeReplyObject(this->reply_);
    return retcode;
  }
}

int Redis::ListPop(const std::string &key, std::string &element) {
  this->reply_ = (redisReply *)redisCommand(this->context_,
      "SPOP %b", key.data(), key.size());
  if(this->reply_ == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Redis] redisCommand occur serious error[%d].",
        __FILE__, __LINE__, __FUNCTION__, this->context_->err);
    this->connecting_ = false;
    redisFree(this->context_);
    this->context_ = NULL;
    return REDIS_SERIOUS_ERROR;
  } else {
    int retcode = 0;
    if(this->reply_->type == REDIS_REPLY_STRING) {
      element.assign(this->reply_->str, this->reply_->len);
      retcode = common::ListResponseType::LIST_RESPONSE_TYPE_SUCCESS;
    } else if(this->reply_->type == REDIS_REPLY_NIL) {
      retcode = common::ListResponseType::LIST_RESPONSE_TYPE_EMPTY;
    } else if(this->reply_->type == REDIS_REPLY_ERROR) {
      retcode = common::ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] list request error [%s].",
          __FILE__, __LINE__, __FUNCTION__, this->reply_->str);
    } else {
      retcode = common::ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] list request unknown error.",
          __FILE__, __LINE__, __FUNCTION__);
    }
    freeReplyObject(this->reply_);
    return retcode;
  }
}

int Redis::ListRemove(const std::string &key, const std::string &element) {
  this->reply_ = (redisReply *)redisCommand(this->context_,
      "SREM %b %b", key.data(), key.size(), element.data(), element.size());
  if(this->reply_ == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Redis] redisCommand occur serious error[%d].",
        __FILE__, __LINE__, __FUNCTION__, this->context_->err);
    this->connecting_ = false;
    redisFree(this->context_);
    this->context_ = NULL;
    return REDIS_SERIOUS_ERROR;
  } else {
    int retcode = 0;
    if(this->reply_->type == REDIS_REPLY_INTEGER) {
      if(this->reply_->integer > 0) {
        retcode = common::ListResponseType::LIST_RESPONSE_TYPE_SUCCESS;
      } else {
        retcode = common::ListResponseType::LIST_RESPONSE_TYPE_ELEM_NOT_EXIST;
      }
    } else if(this->reply_->type == REDIS_SERIOUS_ERROR) {
      retcode = common::ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] list request error [%s].",
          __FILE__, __LINE__, __FUNCTION__, this->reply_->str);
    } else {
      retcode = common::ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] list request unknown error.",
          __FILE__, __LINE__, __FUNCTION__);
    }
    freeReplyObject(this->reply_);
    return retcode;
  }
}

int Redis::ListClear(const std::string &key) {
  return this->ListDelete(key);
}

int Redis::ListDelete(const std::string &key) {
  this->reply_ = (redisReply *)redisCommand(this->context_,
      "DEL %b", key.data(), key.size());
  if(this->reply_ == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Redis] redisCommand occur serious error[%d].",
        __FILE__, __LINE__, __FUNCTION__, this->context_->err);
    this->connecting_ = false;
    redisFree(this->context_);
    this->context_ = NULL;
    return REDIS_SERIOUS_ERROR;
  } else {
    int retcode = 0;
    if(this->reply_->type == REDIS_REPLY_INTEGER) {
      if(this->reply_->integer > 0) {
        retcode = common::ListResponseType::LIST_RESPONSE_TYPE_SUCCESS;
      } else {
        retcode = common::ListResponseType::LIST_RESPONSE_TYPE_KEY_NOT_EXIST;
      }
    } else if(this->reply_->type == REDIS_REPLY_NIL) {
      retcode = common::ListResponseType::LIST_RESPONSE_TYPE_KEY_NOT_EXIST;
    } else if(this->reply_->type == REDIS_REPLY_ERROR) {
      retcode = common::ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] list request error [%s].",
          __FILE__, __LINE__, __FUNCTION__, this->reply_->str);
    } else {
      retcode = common::ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] list request unknown error.",
          __FILE__, __LINE__, __FUNCTION__);
    }
    freeReplyObject(this->reply_);
    return retcode;
  }
}

int Redis::MapInsert(const std::string &key,
    const std::string &score, const std::string &element) {
  this->reply_ = (redisReply *)redisCommand(this->context_,
      "ZADD %b %s %b", key.data(), key.size(), score.c_str(),
      element.data(), element.size());
  if(this->reply_ == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Redis] redisCommand occur serious error[%d].",
        __FILE__, __LINE__, __FUNCTION__, this->context_->err);
    this->connecting_ = false;
    redisFree(this->context_);
    this->context_ = NULL;
    return REDIS_SERIOUS_ERROR;
  } else {
    int retcode = 0;
    if(this->reply_->type == REDIS_REPLY_INTEGER) {
      if(this->reply_->integer > 0) {
        retcode = common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS;
      } else {
        retcode = common::MapResponseType::MAP_RESPONSE_TYPE_ELEM_ALREDY_EXIST;
      }
    } else if(this->reply_->type == REDIS_REPLY_ERROR) {
      retcode = common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] map request error [%s].",
          __FILE__, __LINE__, __FUNCTION__, this->reply_->str);
    } else {
      retcode = common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] map request unknown error.",
          __FILE__, __LINE__, __FUNCTION__);
    }
    freeReplyObject(this->reply_);
    return retcode;
  }
}

int Redis::MapRemove(const std::string &key, const std::string &element) {
  this->reply_ = (redisReply *)redisCommand(this->context_,
      "ZREM %b %b", key.data(), key.size(), element.data(), element.size());
  if(this->reply_ == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Redis] redisCommand occur serious error[%d].",
        __FILE__, __LINE__, __FUNCTION__, this->context_->err);
    this->connecting_ = false;
    redisFree(this->context_);
    this->context_ = NULL;
    return REDIS_SERIOUS_ERROR;
  } else {
    int retcode = 0;
    if(this->reply_->type == REDIS_REPLY_INTEGER) {
      if(this->reply_->integer > 0) {
        retcode = common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS;
      } else {
        retcode = common::MapResponseType::MAP_RESPONSE_TYPE_ELEM_NOT_EXIST;
      }
    } else if(this->reply_->type == REDIS_SERIOUS_ERROR) {
      retcode = common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] map request error [%s].",
          __FILE__, __LINE__, __FUNCTION__, this->reply_->str);
    } else {
      retcode = common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] map request unknown error.",
          __FILE__, __LINE__, __FUNCTION__);
    }
    freeReplyObject(this->reply_);
    return retcode;
  }
}

int Redis::MapQueryRank(const std::string &key, const std::string &element,
    std::string &ranking, bool asc) {
  if(asc) {
    this->reply_ = (redisReply *)redisCommand(this->context_,
        "ZRANK %b %b", key.data(), key.size(), element.data(), element.size());
  } else {
    this->reply_ = (redisReply *)redisCommand(this->context_,
        "ZREVRANK %b %b", key.data(), key.size(), element.data(), element.size());
  }
  if(this->reply_ == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Redis] redisCommand occur serious error[%d].",
        __FILE__, __LINE__, __FUNCTION__, this->context_->err);
    this->connecting_ = false;
    redisFree(this->context_);
    this->context_ = NULL;
    return REDIS_SERIOUS_ERROR;
  } else {
    int retcode = 0;
    if(this->reply_->type == REDIS_REPLY_INTEGER) {
      retcode = common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS;
      char integer_str[32];
      snprintf(integer_str, sizeof(integer_str), "%lld", this->reply_->integer);
      ranking = integer_str;
    } else if(this->reply_->type == REDIS_REPLY_NIL) {
      retcode = common::MapResponseType::MAP_RESPONSE_TYPE_ELEM_NOT_EXIST;
    } else if(this->reply_->type == REDIS_REPLY_ERROR) {
      retcode = common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] map request error [%s].",
          __FILE__, __LINE__, __FUNCTION__, this->reply_->str);
    } else {
      retcode = common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] map request unknown error.",
          __FILE__, __LINE__, __FUNCTION__);
    }
    freeReplyObject(this->reply_);
    return retcode;
  }
}

int Redis::MapQueryRange(const std::string &key, const std::string &rank_start,
    const std::string &rank_end, bool asc) {
  if(asc) {
    this->reply_ = (redisReply *)redisCommand(this->context_,
        "ZRANGE %b %s %s", key.data(), key.size(),
        rank_start.c_str(), rank_end.c_str());
  } else {
    this->reply_ = (redisReply *)redisCommand(this->context_,
        "ZREVRANGE %b %s %s", key.data(), key.size(),
        rank_start.c_str(), rank_end.c_str());
  }
  if(this->reply_ == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Redis] redisCommand occur serious error[%d].",
        __FILE__, __LINE__, __FUNCTION__, this->context_->err);
    this->connecting_ = false;
    redisFree(this->context_);
    this->context_ = NULL;
    return REDIS_SERIOUS_ERROR;
  } else {
    int retcode = 0;
    if(this->reply_->type == REDIS_REPLY_ARRAY) {
      return common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS;
    } else if(this->reply_->type == REDIS_REPLY_ERROR) {
      retcode = common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] map request error [%s].",
          __FILE__, __LINE__, __FUNCTION__, this->reply_->str);
    } else {
      retcode = common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] map request unknown error.",
          __FILE__, __LINE__, __FUNCTION__);
    }
    freeReplyObject(this->reply_);
    return retcode;
  }
}
int Redis::MapQueryRangeWithscores(const std::string &key, const std::string &rank_start,
    const std::string &rank_end, bool asc) {
  if(asc) {
    this->reply_ = (redisReply *)redisCommand(this->context_,
        "ZRANGE %b %s %s WITHSCORES", key.data(), key.size(),
        rank_start.c_str(), rank_end.c_str());
  } else {
    this->reply_ = (redisReply *)redisCommand(this->context_,
        "ZREVRANGE %b %s %s WITHSCORES", key.data(), key.size(),
        rank_start.c_str(), rank_end.c_str());
  }
  if(this->reply_ == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Redis] redisCommand occur serious error[%d].",
        __FILE__, __LINE__, __FUNCTION__, this->context_->err);
    this->connecting_ = false;
    redisFree(this->context_);
    this->context_ = NULL;
    return REDIS_SERIOUS_ERROR;
  } else {
    int retcode = 0;
    if(this->reply_->type == REDIS_REPLY_ARRAY) {
      return common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS;
    } else if(this->reply_->type == REDIS_REPLY_ERROR) {
      retcode = common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] map request error [%s].",
          __FILE__, __LINE__, __FUNCTION__, this->reply_->str);
    } else {
      retcode = common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] map request unknown error.",
          __FILE__, __LINE__, __FUNCTION__);
    }
    freeReplyObject(this->reply_);
    return retcode;
  }
}

bool Redis::MapFetchRangeValue(std::string &element, size_t index) {
  if(this->reply_->type == REDIS_REPLY_ARRAY &&
      index < this->reply_->elements) {
    element.assign(this->reply_->element[index]->str,
        this->reply_->element[index]->len);
    return true;
  }
  return false;
}

bool Redis::MapFetchRangeValueWithscores(std::string &score,
    std::string &element, size_t index) {
  index *= 2;
  if(this->reply_->type == REDIS_REPLY_ARRAY &&
      index + 1 < this->reply_->elements) {
    element.assign(this->reply_->element[index]->str,
        this->reply_->element[index]->len);
    score.assign(this->reply_->element[index + 1]->str,
        this->reply_->element[index + 1]->len);
    return true;
  }
  return false;
}

void Redis::MapQuitFetchRange() {
  freeReplyObject(this->reply_);
}

int Redis::MapResetScore(const std::string &key,
    const std::string &score, const std::string &element) {
  int retcode = this->MapInsert(key, score, element);
  if(retcode == common::MapResponseType::MAP_RESPONSE_TYPE_ELEM_ALREDY_EXIST) {
    retcode = common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS;
  }
  return retcode;
}

int Redis::MapClear(const std::string &key) {
  this->reply_ = (redisReply *)redisCommand(this->context_,
      "ZREMRANGEBYRANK %b 0 -1", key.data(), key.size());
  if(this->reply_ == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Redis] redisCommand occur serious error[%d].",
        __FILE__, __LINE__, __FUNCTION__, this->context_->err);
    this->connecting_ = false;
    redisFree(this->context_);
    this->context_ = NULL;
    return REDIS_SERIOUS_ERROR;
  } else {
    int retcode = 0;
    if(this->reply_->type == REDIS_REPLY_INTEGER) {
      retcode = common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS;
    } else if(this->reply_->type == REDIS_SERIOUS_ERROR) {
      retcode = common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] map request error [%s].",
          __FILE__, __LINE__, __FUNCTION__, this->reply_->str);
    } else {
      retcode = common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] map request unknown error.",
          __FILE__, __LINE__, __FUNCTION__);
    }
    freeReplyObject(this->reply_);
    return retcode;
  }
}

int Redis::MapDelete(const std::string &key) {
  this->reply_ = (redisReply *)redisCommand(this->context_,
      "DEL %b", key.data(), key.size());
  if(this->reply_ == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Redis] redisCommand occur serious error[%d].",
        __FILE__, __LINE__, __FUNCTION__, this->context_->err);
    this->connecting_ = false;
    redisFree(this->context_);
    this->context_ = NULL;
    return REDIS_SERIOUS_ERROR;
  } else {
    int retcode = 0;
    if(this->reply_->type == REDIS_REPLY_INTEGER) {
      if(this->reply_->integer >= 0) {
        retcode = common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS;
      } else {
        retcode = common::MapResponseType::MAP_RESPONSE_TYPE_KEY_NOT_EXIST;
      }
    } else if(this->reply_->type == REDIS_REPLY_NIL) {
      retcode = common::MapResponseType::MAP_RESPONSE_TYPE_KEY_NOT_EXIST;
    } else if(this->reply_->type == REDIS_REPLY_ERROR) {
      retcode = common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] list request error [%s].",
          __FILE__, __LINE__, __FUNCTION__, this->reply_->str);
    } else {
      retcode = common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Redis] list request unknown error.",
          __FILE__, __LINE__, __FUNCTION__);
    }
    freeReplyObject(this->reply_);
    return retcode;
  }
}

}  // namespace redis

}  // namespace dataproxy

}  // namespace squirrel

