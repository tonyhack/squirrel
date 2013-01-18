//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-22 16:28:09.
// File name: list_test.cc
//
// Description: 
// Test of list request.
//

#include <deque>
#include <sstream>

#include "core/configure.h"
#include "core/event_loop.h"
#include "protocol/message_types.h"
#include "protocol/protocol_list_message_types.h"
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

struct ListRequestConfigure {
  std::string key_;
  std::string element_;
  protocol::ListRequestType::type type_;
  protocol::StorageType::type storage_type_;
};

class ListTest : public HttpClient {
  typedef std::deque<ListRequestConfigure> ListRequestQueue;
 public:
  ListTest(const core::InetAddress &server_address,
      const std::string &name) : HttpClient(server_address, name) {
#ifdef _TIME_CONSUMING_TEST
    time_consume_.SetDebugInfo("consumer test");
#endif
  }

  virtual ~ListTest() {}

  virtual bool OnMessage(const char *message, size_t size) {
#ifdef _TIME_CONSUMING_TEST
    this->time_consume_.ConsumeLogging();
#endif
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
        "[ListTest] OnMessage.");
    // Response.
    protocol::Response response;
    if(this->packet_.Deserialize<protocol::Response>(&response, message, size) == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [ListTest] Deserialize Request failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    if(response.data_type_ != protocol::DataType::DATA_TYPE_LIST) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [ListTest] Response not DATA_TYPE_LIST type.",
          __FILE__, __LINE__, __FUNCTION__);
        return false;
    }

    // ListRequest.
    protocol::ListResponse list;
    if(this->packet_.Deserialize<protocol::ListResponse>(&list, response.response_) == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [ListTest] Deserialize ListRequest failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    if(list.result_ != protocol::ListResponseType::LIST_RESPONSE_TYPE_SUCCESS) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [ListTest] list(type=%d) response failed(%d).",
          __FILE__, __LINE__, __FUNCTION__, this->type_, list.result_);
      return false;
    }

    switch(this->type_) {
      case protocol::ListRequestType::LIST_REQUEST_TYPE_PUSH:
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
            "[ListTest] push(%s, %s) response", this->key_.c_str(),
            this->element_.c_str());
        break;

      case protocol::ListRequestType::LIST_REQUEST_TYPE_POP:
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
            "[ListTest] pop(%s) response(%s)", this->key_.c_str(),
            list.data_.c_str());
        break;

      case protocol::ListRequestType::LIST_REQUEST_TYPE_REMOVE:
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
            "[ListTest] remove(%s, %s) response", this->key_.c_str(),
            this->element_.c_str());
        break;

      case protocol::ListRequestType::LIST_REQUEST_TYPE_CLEAR:
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
            "[ListTest] clear(%s) response", this->key_.c_str());
        break;

      case protocol::ListRequestType::LIST_REQUEST_TYPE_DELETE:
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
            "[ListTest] delete(%s) response", this->key_.c_str());
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

  inline void AddRequest(const ListRequestConfigure &request) {
    this->requests_.push_back(request);
  }

 private:
  void DoRequest() {
    if(this->requests_.empty() == false) {
      ListRequestConfigure request = this->requests_.front();
      this->requests_.pop_front();
      switch(request.type_) {
        case protocol::ListRequestType::LIST_REQUEST_TYPE_PUSH:
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
              "[ListTest] push(%s, %s) request", request.key_.c_str(),
              request.element_.c_str());
          this->TestPush(request.key_, request.element_, request.storage_type_);
          break;
        case protocol::ListRequestType::LIST_REQUEST_TYPE_POP:
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
              "[ListTest] pop(%s) request", request.key_.c_str());
          this->TestPop(request.key_, request.storage_type_);
          break;
          break;
        case protocol::ListRequestType::LIST_REQUEST_TYPE_REMOVE:
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
              "[ListTest] reset(%s, %s) request", request.key_.c_str(),
              request.element_.c_str());
          this->TestRemove(request.key_, request.element_, request.storage_type_);
          break;
        case protocol::ListRequestType::LIST_REQUEST_TYPE_CLEAR:
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
              "[ListTest] clear(%s) request", request.key_.c_str());
          this->TestClear(request.key_, request.storage_type_);
          break;
        case protocol::ListRequestType::LIST_REQUEST_TYPE_DELETE:
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
              "[ListTest] delete(%s) request", request.key_.c_str());
          this->TestDelete(request.key_, request.storage_type_);
          break;
        default:
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
              "%s:%d (%s) [ListTest] Error request type(%d).",
              __FILE__, __LINE__, __FUNCTION__, request.type_);
          break;
      }
    }
#ifdef _TIME_CONSUMING_TEST
    this->time_consume_.ResetConsuming();
#endif
  }

  bool TestPush(const std::string &key, const std::string &element,
      protocol::StorageType::type storage_type) {
    protocol::ListRequest list;
    list.__set_type_(protocol::ListRequestType::LIST_REQUEST_TYPE_PUSH);
    list.__set_key_(key);
    list.__set_data_(element);
    size_t size = 0;
    const char *ptr = this->packet_.Serialize<protocol::ListRequest>(&list, size);
    if(ptr) {
      protocol::Request request;
      request.__set_data_type_(protocol::DataType::DATA_TYPE_LIST);
      request.__set_storage_type_(storage_type);
      request.request_.assign(ptr, size);
      ptr = this->packet_.Serialize<protocol::Request>(&request, size);
      if(ptr) {
        this->type_ = list.type_;
        this->key_ = key;
        this->element_ = element;
        this->Post(ptr, size);
        return true;
      } else {
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
            "%s:%d (%s) [ListTest] Serialize Request failed.",
            __FILE__, __LINE__, __FUNCTION__);
        return false;
      }
    } else {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [ListTest] Serialize ListRequest failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
  }

  bool TestPop(const std::string &key, protocol::StorageType::type storage_type) {
    protocol::ListRequest list;
    list.__set_type_(protocol::ListRequestType::LIST_REQUEST_TYPE_POP);
    list.__set_key_(key);
    size_t size = 0;
    const char *ptr = this->packet_.Serialize<protocol::ListRequest>(&list, size);
    if(ptr) {
      protocol::Request request;
      request.__set_data_type_(protocol::DataType::DATA_TYPE_LIST);
      request.__set_storage_type_(storage_type);
      request.request_.assign(ptr, size);
      ptr = this->packet_.Serialize<protocol::Request>(&request, size);
      if(ptr) {
        this->type_ = list.type_;
        this->key_ = key;
        this->Post(ptr, size);
        return true;
      } else {
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
            "%s:%d (%s) [ListTest] Serialize Request failed.",
            __FILE__, __LINE__, __FUNCTION__);
        return false;
      }
    } else {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [ListTest] Serialize ListRequest failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
  }

  bool TestRemove(const std::string &key, const std::string &element,
      protocol::StorageType::type storage_type) {
    protocol::ListRequest list;
    list.__set_type_(protocol::ListRequestType::LIST_REQUEST_TYPE_REMOVE);
    list.__set_key_(key);
    list.__set_data_(element);
    size_t size = 0;
    const char *ptr = this->packet_.Serialize<protocol::ListRequest>(&list, size);
    if(ptr) {
      protocol::Request request;
      request.__set_data_type_(protocol::DataType::DATA_TYPE_LIST);
      request.__set_storage_type_(storage_type);
      request.request_.assign(ptr, size);
      ptr = this->packet_.Serialize<protocol::Request>(&request, size);
      if(ptr) {
        this->type_ = list.type_;
        this->key_ = key;
        this->element_ = element;
        this->Post(ptr, size);
        return true;
      } else {
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
            "%s:%d (%s) [ListTest] Serialize Request failed.",
            __FILE__, __LINE__, __FUNCTION__);
        return false;
      }
    } else {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [ListTest] Serialize ListRequest failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
  }

  bool TestClear(const std::string &key, protocol::StorageType::type storage_type) {
    protocol::ListRequest list;
    list.__set_type_(protocol::ListRequestType::LIST_REQUEST_TYPE_CLEAR);
    list.__set_key_(key);
    size_t size = 0;
    const char *ptr = this->packet_.Serialize<protocol::ListRequest>(&list, size);
    if(ptr) {
      protocol::Request request;
      request.__set_data_type_(protocol::DataType::DATA_TYPE_LIST);
      request.__set_storage_type_(storage_type);
      request.request_.assign(ptr, size);
      ptr = this->packet_.Serialize<protocol::Request>(&request, size);
      if(ptr) {
        this->type_ = list.type_;
        this->key_ = key;
        this->Post(ptr, size);
        return true;
      } else {
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
            "%s:%d (%s) [ListTest] Serialize Request failed.",
            __FILE__, __LINE__, __FUNCTION__);
        return false;
      }
    } else {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [ListTest] Serialize ListRequest failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
  }

  bool TestDelete(const std::string &key, protocol::StorageType::type storage_type) {
    protocol::ListRequest list;
    list.__set_type_(protocol::ListRequestType::LIST_REQUEST_TYPE_DELETE);
    list.__set_key_(key);
    size_t size = 0;
    const char *ptr = this->packet_.Serialize<protocol::ListRequest>(&list, size);
    if(ptr) {
      protocol::Request request;
      request.__set_data_type_(protocol::DataType::DATA_TYPE_LIST);
      request.__set_storage_type_(storage_type);
      request.request_.assign(ptr, size);
      ptr = this->packet_.Serialize<protocol::Request>(&request, size);
      if(ptr) {
        this->type_ = list.type_;
        this->key_ = key;
        this->Post(ptr, size);
        return true;
      } else {
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
            "%s:%d (%s) [ListTest] Serialize Request failed.",
            __FILE__, __LINE__, __FUNCTION__);
        return false;
      }
    } else {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [ListTest] Serialize ListRequest failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
  }

  global::ThriftPacket packet_;

  protocol::ListRequestType::type type_;
  std::string key_;
  std::string element_;

  ListRequestQueue requests_;

#ifdef _TIME_CONSUMING_TEST
  global::TimeConsuming time_consume_;
#endif
};

void TestPush(ListTest &test, int from, int to, bool redis = true) {
  ListRequestConfigure request;
  std::ostringstream oss;
  request.key_ = "TEST-LIST-001";
  for(int i = from; i < to; ++i) {
    oss.str("");
    oss << "element-" << i;
    request.element_ = oss.str();
    request.type_ = protocol::ListRequestType::LIST_REQUEST_TYPE_PUSH;
    request.storage_type_ = redis ? protocol::StorageType::STORAGE_TYPE_CACHE :
      protocol::StorageType::STORAGE_TYPE_DISK;
    test.AddRequest(request);
  }
}

void TestPop(ListTest &test, int from, int to, bool redis = true) {
  ListRequestConfigure request;
  std::ostringstream oss;
  request.key_ = "TEST-LIST-001";
  for(int i = from; i < to; ++i) {
    oss.str("");
    request.type_ = protocol::ListRequestType::LIST_REQUEST_TYPE_POP;
    request.storage_type_ = redis ? protocol::StorageType::STORAGE_TYPE_CACHE :
      protocol::StorageType::STORAGE_TYPE_DISK;
    test.AddRequest(request);
  }
}

void TestRemove(ListTest &test, int from, int to, bool redis = true) {
  ListRequestConfigure request;
  std::ostringstream oss;
  request.key_ = "TEST-LIST-001";
  for(int i = from; i < to; ++i) {
    oss.str("");
    oss << "element-" << i;
    request.element_ = oss.str();
    request.type_ = protocol::ListRequestType::LIST_REQUEST_TYPE_REMOVE;
    request.storage_type_ = redis ? protocol::StorageType::STORAGE_TYPE_CACHE :
      protocol::StorageType::STORAGE_TYPE_DISK;
    test.AddRequest(request);
  }
}

void TestClear(ListTest &test, bool redis = true) {
  ListRequestConfigure request;
  std::ostringstream oss;
  request.key_ = "TEST-LIST-001";
  request.type_ = protocol::ListRequestType::LIST_REQUEST_TYPE_CLEAR;
  request.storage_type_ = redis ? protocol::StorageType::STORAGE_TYPE_CACHE :
    protocol::StorageType::STORAGE_TYPE_DISK;
  test.AddRequest(request);
}

void TestDelete(ListTest &test, bool redis = true) {
  ListRequestConfigure request;
  std::ostringstream oss;
  request.key_ = "TEST-LIST-001";
  request.type_ = protocol::ListRequestType::LIST_REQUEST_TYPE_DELETE;
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

  ListTest test(core::InetAddress(host, port), "ListTest");
  if(test.Initialize(&loop) == false) {
    printf("[ListTest] Initialize failed.\n");
    return 0;
  }
  printf("[ListTest] Initialize\n");



  // redis test.
  TestDelete(test);

  TestPush(test, 0, 99);
  TestPop(test, 0, 99);

  TestPush(test, 0, 99);
  TestRemove(test, 0, 99);

  TestPush(test, 0, 99);
  TestClear(test);

  TestPush(test, 0, 99);
  TestDelete(test);


  // storage test.
  TestDelete(test, false);

  TestPush(test, 0, 99, false);
  TestPop(test, 0, 99, false);

  TestPush(test, 0, 99, false);
  TestRemove(test, 0, 99, false);

  TestPush(test, 0, 99, false);
  TestClear(test, false);

  TestPush(test, 0, 99, false);
  TestDelete(test, false);

  // TestPush(test, 0, 99, false);


  printf("[KeyvalueTest] Add request\n");

  test.Start();

  printf("[KeyvalueTest] Start\n");

  loop.Loop();

  global::AsyncLoggingSingleton::GetAsyncLogging()->Stop();

  return 0;
}

