//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-10-02 16:53:57.
// File name: map_test.cc
//
// Description: 
// Test of map request.
//

#include <deque>
#include <sstream>

#include "core/configure.h"
#include "core/event_loop.h"
#include "protocol/message_types.h"
#include "protocol/protocol_map_message_types.h"
#include "global/async_logging_singleton.h"
#include "global/time_consuming.h"
#include "global/thrift_packet.h"
#include "test/http_client.h"

using namespace squirrel;
using namespace squirrel::global;
using namespace squirrel::test;

#ifndef _TIME_CONSUMING_TEST
#define _TIME_CONSUMING_TEST
#endif  // _TIME_CONSUMING_TEST

struct MapRequestConfigure {
  std::string key_;
  std::string ranking_key_;
  std::string element_;

  std::string start_;
  std::string end_;

  protocol::MapRequestType::type type_;
  protocol::MapSortType::type sort_type_;
  protocol::StorageType::type storage_type_;
};

class MapTest : public HttpClient {
  typedef std::deque<MapRequestConfigure> MapRequestQueue;
 public:
  MapTest(const core::InetAddress &server_address,
      const std::string &name) : HttpClient(server_address, name) {
#ifdef _TIME_CONSUMING_TEST
    time_consume_.SetDebugInfo("consumer test");
#endif
  }

  virtual ~MapTest() {}

  virtual bool OnMessage(const char *message, size_t size) {
#ifdef _TIME_CONSUMING_TEST
    this->time_consume_.ConsumeLogging();
#endif
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
        "[MapTest] OnMessage.");
    // Response.
    protocol::Response response;
    if(this->packet_.Deserialize<protocol::Response>(&response, message, size) == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [MapTest] Deserialize Request failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    if(response.data_type_ != protocol::DataType::DATA_TYPE_MAP) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [MapTest] Response not DATA_TYPE_MAP type.",
          __FILE__, __LINE__, __FUNCTION__);
        return false;
    }

    // MapResponse.
    protocol::MapResponse map;
    if(this->packet_.Deserialize<protocol::MapResponse>(&map, response.response_) == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [MapTest] Deserialize MapRequest failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    if(map.type_ != protocol::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [MapTest] map(type=%d) response failed(%d).",
          __FILE__, __LINE__, __FUNCTION__, this->type_, map.type_);
      return false;
    }

    switch(this->type_) {
      case protocol::MapRequestType::MAP_REQUEST_TYPE_INSERT:
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
            "[MapTest] Insert(key[%s], element[%s], score[%s]) response", this->key_.c_str(),
            this->element_.c_str(), this->score_.c_str());
        break;
      case protocol::MapRequestType::MAP_REQUEST_TYPE_REMOVE:
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
            "[MapTest] Remove(key[%s], element[%s]) response", this->key_.c_str(),
            this->element_.c_str());
        break;
      case protocol::MapRequestType::MAP_REQUEST_TYPE_QUERY_RANKING:
        {
          protocol::MapQueryRankingResponse ranking;
          if(this->packet_.Deserialize<protocol::MapQueryRankingResponse>(&ranking, map.data_) == false) {
            AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
                "%s:%d (%s) [MapTest] Deserialize MapQueryRankingResponse failed.",
                __FILE__, __LINE__, __FUNCTION__);
            return false;
          }
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
              "[MapTest] QueryRanking(key[%s], element[%s], ranking[%s]) response",
              this->key_.c_str(), this->element_.c_str(), ranking.ranking_.c_str());
        }
        break;
      case protocol::MapRequestType::MAP_REQUEST_TYPE_QUERY_RANKING_RANGE:
        {
          protocol::MapQueryRankingRangeResponse range;
          if(this->packet_.Deserialize<protocol::MapQueryRankingRangeResponse>(&range, map.data_) == false) {
            AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
                "%s:%d (%s) [MapTest] Deserialize MapQueryRankingRangeResponse failed.",
                __FILE__, __LINE__, __FUNCTION__);
            return false;
          }
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
              "[MapTest] QueryRank(key[%s], start[%s], end[%s]) ------------- begin ---------------",
              this->key_.c_str(), this->start_.c_str(), this->end_.c_str());
          typedef std::vector<protocol::MapQueryRankingRangeInfo> RangeVector;
          RangeVector::iterator iterator = range.elements_.begin();
          for(; iterator != range.elements_.end(); ++iterator) {
            AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
                "[MapTest]    element(element[%s], ranking[%s], score[%s])",
                iterator->elements_.c_str(), iterator->ranking_.c_str(), iterator->ranking_key_.c_str());
          }
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
              "[MapTest] QueryRank(key[%s], start[%s], end[%s]) -------------  end  ---------------",
              this->key_.c_str(), this->start_.c_str(), this->end_.c_str());
        }
        break;
      case protocol::MapRequestType::MAP_REQUEST_TYPE_RESET_RANKING_KEY:
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
            "[MapTest] ResetScore(key[%s], element[%s], score[%s]) response", this->key_.c_str(),
            this->element_.c_str(), this->score_.c_str());
        break;
      case protocol::MapRequestType::MAP_REQUEST_TYPE_CLEAR:
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
            "[MapTest] Clear(key[%s]) response", this->key_.c_str());
        break;
      case protocol::MapRequestType::MAP_REQUEST_TYPE_DELETE:
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
            "[MapTest] Delete(key[%s]) response", this->key_.c_str());
        break;
      default:
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
            "[ListTest] Response error type(%d)", this->type_);
        return false;
    }

    this->DoRequest();
    return true;
  }

  virtual void OnConnected() {
    this->DoRequest();
  }

  virtual void OnDisconnected() {
  }

  inline void AddRequest(const MapRequestConfigure &request) {
    this->requests_.push_back(request);
  }

 private:
  void DoRequest() {
    if(this->requests_.empty() == false) {
      MapRequestConfigure request = this->requests_.front();
      this->requests_.pop_front();
      switch(request.type_) {
        case protocol::MapRequestType::MAP_REQUEST_TYPE_INSERT:
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
              "[MapTest] insert(key[%s], element[%s], score[%s]) request",
              request.key_.c_str(), request.element_.c_str(), request.ranking_key_.c_str());
          this->TestInsert(request.key_, request.element_,
              request.ranking_key_, request.storage_type_);
          break;
        case protocol::MapRequestType::MAP_REQUEST_TYPE_REMOVE:
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
              "[MapTest] remove(key[%s], element[%s]) request",
              request.key_.c_str(), request.element_.c_str());
          this->TestRemove(request.key_, request.element_, request.storage_type_);
          break;
        case protocol::MapRequestType::MAP_REQUEST_TYPE_QUERY_RANKING:
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
              "[MapTest] query ranking(key[%s], element[%s], sort[%d]) request",
              request.key_.c_str(), request.element_.c_str(), request.sort_type_);
          this->TestQueryRanking(request.key_, request.element_,
              request.sort_type_, request.storage_type_);
          break;
        case protocol::MapRequestType::MAP_REQUEST_TYPE_QUERY_RANKING_RANGE:
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
              "[MapTest] query range(key[%s], start[%s], end[%s], sort[%d]) request",
              request.key_.c_str(), request.start_.c_str(), request.end_.c_str(),
              request.sort_type_);
          this->TestQueryRange(request.key_, request.start_, request.end_,
              request.sort_type_, request.storage_type_);
          break;
        case protocol::MapRequestType::MAP_REQUEST_TYPE_RESET_RANKING_KEY:
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
              "[MapTest] reset ranking(key[%s], element[%s], score[%s])",
              request.key_.c_str(), request.element_.c_str(), request.ranking_key_.c_str());
          this->TestResetScore(request.key_, request.element_,
              request.ranking_key_, request.storage_type_);
          break;
        case protocol::MapRequestType::MAP_REQUEST_TYPE_CLEAR:
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
              "[MapTest] clear(key[%s])", request.key_.c_str());
          this->TestClear(request.key_, request.storage_type_);
          break;
        case protocol::MapRequestType::MAP_REQUEST_TYPE_DELETE:
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
              "[MapTest] delete(key[%s])", request.key_.c_str());
          this->TestDelete(request.key_, request.storage_type_);
          break;
        default:
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
              "%s:%d (%s) [MapTest] Error request type(%d).",
              __FILE__, __LINE__, __FUNCTION__, request.type_);
          break;
      }
    }
#ifdef _TIME_CONSUMING_TEST
    this->time_consume_.ResetConsuming();
#endif
  }

  bool TestInsert(const std::string &key, const std::string &element,
      const std::string &ranking_key, protocol::StorageType::type storage_type) {
    protocol::MapInsertRequest insert;
    insert.__set_ranking_key_(ranking_key);
    insert.__set_element_(element);
    size_t size = 0;
    const char *ptr = this->packet_.Serialize<protocol::MapInsertRequest>(&insert, size);
    if(ptr == NULL) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [MapTest] Serialize MapInsertRequest failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    // Map request.
    protocol::MapRequest map;
    map.__set_type_(protocol::MapRequestType::MAP_REQUEST_TYPE_INSERT);
    map.__set_key_(key);
    map.__isset.data_ = true;
    map.data_.assign(ptr, size);
    ptr = this->packet_.Serialize<protocol::MapRequest>(&map, size);
    if(ptr == NULL) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [MapTest] Serialize MapRequest failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    // request.
    protocol::Request request;
    request.__set_data_type_(protocol::DataType::DATA_TYPE_MAP);
    request.__set_storage_type_(storage_type);
    request.request_.assign(ptr, size);
    ptr = this->packet_.Serialize<protocol::Request>(&request, size);
    if(ptr == NULL) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [MapTest] Serialize Request failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    this->type_ = map.type_;
    this->key_ = key;
    this->element_ = element;
    this->score_ = ranking_key;
    this->Post(ptr, size);
    return true;
  }

  bool TestRemove(const std::string &key, const std::string &element,
      protocol::StorageType::type storage_type) {
    protocol::MapRemoveRequest remove;
    remove.__set_element_(element);
    size_t size = 0;
    const char *ptr = this->packet_.Serialize<protocol::MapRemoveRequest>(&remove, size);
    if(ptr == NULL) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [MapTest] Serialize MapRemoveRequest failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    // Map request.
    protocol::MapRequest map;
    map.__set_type_(protocol::MapRequestType::MAP_REQUEST_TYPE_REMOVE);
    map.__set_key_(key);
    map.__isset.data_ = true;
    map.data_.assign(ptr, size);
    ptr = this->packet_.Serialize<protocol::MapRequest>(&map, size);
    if(ptr == NULL) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [MapTest] Serialize MapRequest failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    // request.
    protocol::Request request;
    request.__set_data_type_(protocol::DataType::DATA_TYPE_MAP);
    request.__set_storage_type_(storage_type);
    request.request_.assign(ptr, size);
    ptr = this->packet_.Serialize<protocol::Request>(&request, size);
    if(ptr == NULL) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [MapTest] Serialize Request failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    this->type_ = map.type_;
    this->key_ = key;
    this->element_ = element;
    this->Post(ptr, size);
    return true;
  }

  bool TestQueryRanking(const std::string &key, const std::string &element,
      protocol::MapSortType::type sort_type, protocol::StorageType::type storage_type) {
    protocol::MapQueryRankingRequest query;
    query.__set_element_(element);
    query.__set_sort_type_(sort_type);
    size_t size = 0;
    const char *ptr = this->packet_.Serialize<protocol::MapQueryRankingRequest>(&query, size);
    if(ptr == NULL) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [MapTest] Serialize MapQueryRankingRequest failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    // Map request.
    protocol::MapRequest map;
    map.__set_type_(protocol::MapRequestType::MAP_REQUEST_TYPE_QUERY_RANKING);
    map.__set_key_(key);
    map.__isset.data_ = true;
    map.data_.assign(ptr, size);
    ptr = this->packet_.Serialize<protocol::MapRequest>(&map, size);
    if(ptr == NULL) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [MapTest] Serialize MapRequest failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    // request.
    protocol::Request request;
    request.__set_data_type_(protocol::DataType::DATA_TYPE_MAP);
    request.__set_storage_type_(storage_type);
    request.request_.assign(ptr, size);
    ptr = this->packet_.Serialize<protocol::Request>(&request, size);
    if(ptr == NULL) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [MapTest] Serialize Request failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    this->type_ = map.type_;
    this->key_ = key;
    this->element_ = element;
    this->Post(ptr, size);
    return true;
  }

  bool TestQueryRange(const std::string &key, const std::string &start, const std::string &end,
      protocol::MapSortType::type sort_type, protocol::StorageType::type storage_type) {
    protocol::MapQueryRankingRangeRequest query;
    query.__set_start_(start);
    query.__set_end_(end);
    query.__set_sort_type_(sort_type);
    size_t size = 0;
    const char *ptr = this->packet_.Serialize<protocol::MapQueryRankingRangeRequest>(&query, size);
    if(ptr == NULL) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [MapTest] Serialize MapQueryRankingRangeRequest failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    // Map request.
    protocol::MapRequest map;
    map.__set_type_(protocol::MapRequestType::MAP_REQUEST_TYPE_QUERY_RANKING_RANGE);
    map.__set_key_(key);
    map.__isset.data_ = true;
    map.data_.assign(ptr, size);
    ptr = this->packet_.Serialize<protocol::MapRequest>(&map, size);
    if(ptr == NULL) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [MapTest] Serialize MapRequest failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    // request.
    protocol::Request request;
    request.__set_data_type_(protocol::DataType::DATA_TYPE_MAP);
    request.__set_storage_type_(storage_type);
    request.request_.assign(ptr, size);
    ptr = this->packet_.Serialize<protocol::Request>(&request, size);
    if(ptr == NULL) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [MapTest] Serialize Request failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    this->type_ = map.type_;
    this->key_ = key;
    this->start_ = start;
    this->end_ = end;
    this->Post(ptr, size);
    return true;
  }

  bool TestResetScore(const std::string &key, const std::string &element,
      const std::string &ranking_key, protocol::StorageType::type storage_type) {
    protocol::MapResetRankingKeyRequest reset;
    reset.__set_ranking_key_(ranking_key);
    reset.__set_element_(element);
    size_t size = 0;
    const char *ptr = this->packet_.Serialize<protocol::MapResetRankingKeyRequest>(&reset, size);
    if(ptr == NULL) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [MapTest] Serialize MapResetRankingKeyRequest failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    // Map request.
    protocol::MapRequest map;
    map.__set_type_(protocol::MapRequestType::MAP_REQUEST_TYPE_RESET_RANKING_KEY);
    map.__set_key_(key);
    map.__isset.data_ = true;
    map.data_.assign(ptr, size);
    ptr = this->packet_.Serialize<protocol::MapRequest>(&map, size);
    if(ptr == NULL) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [MapTest] Serialize MapRequest failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    // request.
    protocol::Request request;
    request.__set_data_type_(protocol::DataType::DATA_TYPE_MAP);
    request.__set_storage_type_(storage_type);
    request.request_.assign(ptr, size);
    ptr = this->packet_.Serialize<protocol::Request>(&request, size);
    if(ptr == NULL) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [MapTest] Serialize Request failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    this->type_ = map.type_;
    this->key_ = key;
    this->element_ = element;
    this->score_ = ranking_key;
    this->Post(ptr, size);
    return true;
  }

  bool TestClear(const std::string &key, protocol::StorageType::type storage_type) {
    // Map request.
    protocol::MapRequest map;
    map.__set_type_(protocol::MapRequestType::MAP_REQUEST_TYPE_CLEAR);
    map.__set_key_(key);
    size_t size = 0;
    const char *ptr = this->packet_.Serialize<protocol::MapRequest>(&map, size);
    if(ptr == NULL) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [MapTest] Serialize MapRequest failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    // request.
    protocol::Request request;
    request.__set_data_type_(protocol::DataType::DATA_TYPE_MAP);
    request.__set_storage_type_(storage_type);
    request.request_.assign(ptr, size);
    ptr = this->packet_.Serialize<protocol::Request>(&request, size);
    if(ptr == NULL) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [MapTest] Serialize Request failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    this->type_ = map.type_;
    this->key_ = key;
    this->Post(ptr, size);
    return true;
  }

  bool TestDelete(const std::string &key, protocol::StorageType::type storage_type) {
    // Map request.
    protocol::MapRequest map;
    map.__set_type_(protocol::MapRequestType::MAP_REQUEST_TYPE_DELETE);
    map.__set_key_(key);
    size_t size = 0;
    const char *ptr = this->packet_.Serialize<protocol::MapRequest>(&map, size);
    if(ptr == NULL) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [MapTest] Serialize MapRequest failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    // request.
    protocol::Request request;
    request.__set_data_type_(protocol::DataType::DATA_TYPE_MAP);
    request.__set_storage_type_(storage_type);
    request.request_.assign(ptr, size);
    ptr = this->packet_.Serialize<protocol::Request>(&request, size);
    if(ptr == NULL) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [MapTest] Serialize Request failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    this->type_ = map.type_;
    this->key_ = key;
    this->Post(ptr, size);
    return true;
  }

  global::ThriftPacket packet_;

  protocol::MapRequestType::type type_;
  std::string key_;
  std::string element_;
  std::string score_;
  std::string start_;
  std::string end_;

  MapRequestQueue requests_;

#ifdef _TIME_CONSUMING_TEST
  global::TimeConsuming time_consume_;
#endif
};

  std::string key_;
  std::string ranking_key_;
  std::string element_;

  std::string start_;
  std::string end_;

  protocol::MapRequestType::type type_;
  protocol::MapSortType::type sort_type_;
  protocol::StorageType::type storage_type_;

void TestInsert(MapTest &test, int from, int to, bool redis = true) {
  MapRequestConfigure request;
  std::ostringstream oss;
  request.key_ = "TEST-MAP-001";
  for(int i = from; i < to; ++i) {
    oss.str("");
    oss << "element-" << i;
    request.element_ = oss.str();
    oss.str("");
    oss << i;
    request.ranking_key_ = oss.str();
    request.type_ = protocol::MapRequestType::MAP_REQUEST_TYPE_INSERT;
    request.storage_type_ = redis ? protocol::StorageType::STORAGE_TYPE_CACHE :
      protocol::StorageType::STORAGE_TYPE_DISK;
    test.AddRequest(request);
  }
}

void TestRemove(MapTest &test, int from, int to, bool redis = true) {
  MapRequestConfigure request;
  std::ostringstream oss;
  request.key_ = "TEST-MAP-001";
  for(int i = from; i < to; ++i) {
    oss.str("");
    oss << "element-" << i;
    request.element_ = oss.str();
    oss.str("");
    oss << i;
    request.ranking_key_ = oss.str();
    request.type_ = protocol::MapRequestType::MAP_REQUEST_TYPE_REMOVE;
    request.storage_type_ = redis ? protocol::StorageType::STORAGE_TYPE_CACHE :
      protocol::StorageType::STORAGE_TYPE_DISK;
    test.AddRequest(request);
  }
}

void TestQueryRanking(MapTest &test, int from, int to, bool redis = true) {
  MapRequestConfigure request;
  std::ostringstream oss;
  request.key_ = "TEST-MAP-001";
  for(int i = from; i < to; ++i) {
    oss.str("");
    oss << "element-" << i;
    request.element_ = oss.str();
    oss.str("");
    oss << i;
    request.ranking_key_ = oss.str();
    request.type_ = protocol::MapRequestType::MAP_REQUEST_TYPE_QUERY_RANKING;
    request.storage_type_ = redis ? protocol::StorageType::STORAGE_TYPE_CACHE :
      protocol::StorageType::STORAGE_TYPE_DISK;
    test.AddRequest(request);
  }
}

void TestQueryRange(MapTest &test, int from, int to, bool redis = true) {
  MapRequestConfigure request;
  std::ostringstream oss;
  request.key_ = "TEST-MAP-001";
  oss.str("");
  oss << from;
  request.start_ = oss.str();
  oss.str("");
  oss << to;
  request.end_ = oss.str();
  request.type_ = protocol::MapRequestType::MAP_REQUEST_TYPE_QUERY_RANKING_RANGE;
  request.storage_type_ = redis ? protocol::StorageType::STORAGE_TYPE_CACHE :
    protocol::StorageType::STORAGE_TYPE_DISK;
  test.AddRequest(request);
}

void TestResetScore(MapTest &test, int from, int to, int plus, bool redis = true) {
  MapRequestConfigure request;
  std::ostringstream oss;
  request.key_ = "TEST-MAP-001";
  for(int i = from; i < to; ++i) {
    oss.str("");
    oss << "element-" << i;
    request.element_ = oss.str();
    oss.str("");
    oss << i + plus;
    request.ranking_key_ = oss.str();
    request.type_ = protocol::MapRequestType::MAP_REQUEST_TYPE_RESET_RANKING_KEY;
    request.storage_type_ = redis ? protocol::StorageType::STORAGE_TYPE_CACHE :
      protocol::StorageType::STORAGE_TYPE_DISK;
    test.AddRequest(request);
  }
}

void TestClear(MapTest &test, bool redis = true) {
  MapRequestConfigure request;
  std::ostringstream oss;
  request.key_ = "TEST-MAP-001";
  request.type_ = protocol::MapRequestType::MAP_REQUEST_TYPE_CLEAR;
  request.storage_type_ = redis ? protocol::StorageType::STORAGE_TYPE_CACHE :
    protocol::StorageType::STORAGE_TYPE_DISK;
  test.AddRequest(request);
}

void TestDelete(MapTest &test, bool redis = true) {
  MapRequestConfigure request;
  std::ostringstream oss;
  request.key_ = "TEST-MAP-001";
  request.type_ = protocol::MapRequestType::MAP_REQUEST_TYPE_DELETE;
  request.storage_type_ = redis ? protocol::StorageType::STORAGE_TYPE_CACHE :
    protocol::StorageType::STORAGE_TYPE_DISK;
  test.AddRequest(request);
}

int main(int argc, char *argv[]) {
  if(argc != 4) {
    printf("Bad argument.\n");
    return 0;
  }

  if(global::AsyncLoggingSingleton::GetAsyncLogging()->Initialize(
        std::string(argv[0]) + ".log", 64 * 1024) == false) {
    printf("Initialize logging failed.\n");
    return 0;
  }
  global::AsyncLoggingSingleton::GetAsyncLogging()->Start();

  std::string host;
  core::uint16 port;

  host = argv[1];
  port = atoi(argv[2]);

  core::Configure::Init(argv[3]);
  if(core::Configure::CheckFinishLoad() == false) {
    std::cout << "load configure error!!" << std::endl;
    return 0;
  }

  core::EventLoop loop;
  if(loop.Initialize(true) == false) {
    printf("[ListTest] Initialize EventLoop failed.\n");
    return 0;
  }

  MapTest test(core::InetAddress(host, port), "MapTest");
  if(test.Initialize(&loop) == false) {
    printf("[MapTest] Initialize failed.\n");
    return 0;
  }
  printf("[MapTest] Initialize\n");


  // redis test.
  TestDelete(test);

  TestInsert(test, 0, 99);
  TestRemove(test, 0, 99);

  TestInsert(test, 0, 99);
  TestQueryRanking(test, 0, 99);
  TestQueryRange(test, 0, 99);
  TestResetScore(test, 0, 99, 100);

  TestClear(test);
  TestDelete(test);


  // storage test.
  TestDelete(test, false);

  TestInsert(test, 0, 99, false);
  TestRemove(test, 0, 99, false);

  TestInsert(test, 0, 99, false);
  TestQueryRanking(test, 0, 99, false);
  TestQueryRange(test, 0, 99, false);
  TestResetScore(test, 0, 99, 100, false);

  TestClear(test, false);
  TestDelete(test, false);


  printf("[MapTest] Add request\n");

  test.Start();

  printf("[MapTest] Start\n");

  loop.Loop();

  global::AsyncLoggingSingleton::GetAsyncLogging()->Stop();



  return 0;
}

