//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-14 17:06:14.
// File name: keyvalue_test.cc
//
// Description: 
// Test of keyvalue.
//

#include <deque>
#include <sstream>

#include "core/configure.h"
#include "core/event_loop.h"
#include "protocol/message_types.h"
#include "protocol/protocol_keyvalue_message_types.h"
#include "global/async_logging_singleton.h"
#include "global/time_consuming.h"
#include "global/thrift_packet.h"
#include "test/http_client.h"

using namespace squirrel;
using namespace squirrel::global;
using namespace squirrel::test;

#ifndef _TIME_CONSUMING_TEST
#define _TIME_CONSUMING_TEST
#endif

struct KeyvalueRequestConfigure {
  std::string key_;
  std::string value_;
  protocol::KeyvalueRequestType::type type_;
};

class KeyvalueTest : public HttpClient {
  typedef std::deque<KeyvalueRequestConfigure> KeyvalueRequestQueue;
 public:
  KeyvalueTest(const core::InetAddress &server_address,
      const std::string &name) : HttpClient(server_address, name) {
#ifdef _TIME_CONSUMING_TEST
      time_consume_.SetDebugInfo("consumer test");
#endif
  }

  virtual ~KeyvalueTest() {}

  virtual bool OnMessage(const char *message, size_t size) {
#ifdef _TIME_CONSUMING_TEST
    this->time_consume_.ConsumeLogging();
#endif
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
        "[KeyvalueTest] OnMessage.");
    // Response.
    protocol::Response response;
    if(this->packet_.Deserialize<protocol::Response>(&response, message, size)) {
      if(response.data_type_ != protocol::DataType::DATA_TYPE_KEYVALUE) {
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
            "%s:%d (%s) [KeyvalueTest] Response not DATA_TYPE_KEYVALUE type.",
            __FILE__, __LINE__, __FUNCTION__);
        return false;
      }

      // KeyvalueResponse.
      protocol::KeyvalueResponse keyvalue;
      if(this->packet_.Deserialize<protocol::KeyvalueResponse>(&keyvalue, response.response_)) {
        if(keyvalue.result_ != protocol::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_SUCCESS) {
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
              "%s:%d (%s) [KeyvalueTest] keyvalue(type=%d) response failed(%d).",
              __FILE__, __LINE__, __FUNCTION__, this->type_, keyvalue.result_);
          return false;
        }

        switch(this->type_) {
          case protocol::KeyvalueRequestType::KEYVALUE_REQUEST_TYPE_SET:
            AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
                "[KeyvalueTest] set(%s, %s) response", this->key_.c_str(), this->value_.c_str());
            break;

          case protocol::KeyvalueRequestType::KEYVALUE_REQUEST_TYPE_GET:
            if(keyvalue.value_list_.size() > 0) {
              AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
                  "[KeyvalueTest] get(%s) response value(%s) ", this->key_.c_str(),
                  keyvalue.value_list_[0].c_str());
            } else {
              AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
                  "%s:%d (%s) [KeyvalueTest] KEYVALUE_REQUEST_TYPE_GET response no value.",
                  __FILE__, __LINE__, __FUNCTION__);
              return false;
            }
            break;

          case protocol::KeyvalueRequestType::KEYVALUE_REQUEST_TYPE_DELETE:
            AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
                "[KeyvalueTest] delete(%s) response success", this->key_.c_str());
            break;

          default:
            AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
                "[KeyvalueTest] Response error type(%d)", this->type_);
            return false;
        }

        this->DoRequest();
        return true;
      } else {
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
            "%s:%d (%s) [KeyvalueTest] Deserialize KeyvalueResponse failed.",
            __FILE__, __LINE__, __FUNCTION__);
        return false;
      }
    } else {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [KeyvalueTest] Deserialize KeyvalueResponse failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
  }

  virtual void OnConnected() {
    this->DoRequest();
  }

  virtual void OnDisconnected() {
  }

  inline void AddRequest(const KeyvalueRequestConfigure &request) {
    this->requests_.push_back(request);
  }

 private:
  void DoRequest() {
    if(this->requests_.empty() == false) {
      KeyvalueRequestConfigure request = this->requests_.front();
      this->requests_.pop_front();
      switch(request.type_) {
        case protocol::KeyvalueRequestType::KEYVALUE_REQUEST_TYPE_SET:
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
              "[KeyvalueTest] Set(%s, %s) request", request.key_.c_str(), request.value_.c_str());
          this->TestSet(request.key_, request.value_);
          break;
        case protocol::KeyvalueRequestType::KEYVALUE_REQUEST_TYPE_GET:
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
              "[KeyvalueTest] Get(%s) request", request.key_.c_str());
          this->TestGet(request.key_);
          break;
        case protocol::KeyvalueRequestType::KEYVALUE_REQUEST_TYPE_DELETE:
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
              "[KeyvalueTest] Delete(%s) request", request.key_.c_str());
          this->TestDelete(request.key_);
          break;
        default:
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
              "%s:%d (%s) [KeyvalueTest] Error request type(%d).",
              __FILE__, __LINE__, __FUNCTION__, request.type_);
          break;
      }
    }
#ifdef _TIME_CONSUMING_TEST
    this->time_consume_.ResetConsuming();
#endif
  }

  bool TestSet(const std::string &key, const std::string &value) {
    protocol::KeyvalueRequest keyvalue;
    keyvalue.__set_type_(protocol::KeyvalueRequestType::KEYVALUE_REQUEST_TYPE_SET);
    keyvalue.key_list_.push_back(key);
    keyvalue.value_list_.push_back(value);
    keyvalue.__isset.value_list_ = true;
    size_t size = 0;
    const char *ptr = this->packet_.Serialize<protocol::KeyvalueRequest>(&keyvalue, size);
    if(ptr) {
      protocol::Request request;
      request.__set_data_type_(protocol::DataType::DATA_TYPE_KEYVALUE);
      request.__set_storage_type_(protocol::StorageType::type(protocol::StorageType::STORAGE_TYPE_CACHE |
          protocol::StorageType::STORAGE_TYPE_DISK));
      request.request_.assign(ptr, size);
      const char *ptr = this->packet_.Serialize<protocol::Request>(&request, size);
      if(ptr) {
        this->type_ = keyvalue.type_;
        this->key_ = key;
        this->value_ = value;
        this->Post(ptr, size);
        return true;
      } else {
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
            "%s:%d (%s) [KeyvalueTest] Serialize Request failed.",
            __FILE__, __LINE__, __FUNCTION__);
        return false;
      }
    } else {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [KeyvalueTest] Serialize KeyvalueRequest failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
  }

  bool TestGet(const std::string &key) {
    protocol::KeyvalueRequest keyvalue;
    keyvalue.__set_type_(protocol::KeyvalueRequestType::KEYVALUE_REQUEST_TYPE_GET);
    keyvalue.key_list_.push_back(key);
    size_t size = 0;
    const char *ptr = this->packet_.Serialize<protocol::KeyvalueRequest>(&keyvalue, size);
    if(ptr) {
      protocol::Request request;
      request.__set_data_type_(protocol::DataType::DATA_TYPE_KEYVALUE);
      request.__set_storage_type_(protocol::StorageType::type(protocol::StorageType::STORAGE_TYPE_CACHE |
          protocol::StorageType::STORAGE_TYPE_DISK));
      request.request_.assign(ptr, size);
      const char *ptr = this->packet_.Serialize<protocol::Request>(&request, size);
      if(ptr) {
        this->type_ = keyvalue.type_;
        this->key_ = key;
        this->Post(ptr, size);
        return true;
      } else {
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
            "%s:%d (%s) [KeyvalueTest] Serialize Request failed.",
            __FILE__, __LINE__, __FUNCTION__);
        return false;
      }
    } else {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [KeyvalueTest] Serialize KeyvalueRequest failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
  }

  bool TestDelete(const std::string &key) {
    protocol::KeyvalueRequest keyvalue;
    keyvalue.__set_type_(protocol::KeyvalueRequestType::KEYVALUE_REQUEST_TYPE_DELETE);
    keyvalue.key_list_.push_back(key);
    size_t size = 0;
    const char *ptr = this->packet_.Serialize<protocol::KeyvalueRequest>(&keyvalue, size);
    if(ptr) {
      protocol::Request request;
      request.__set_data_type_(protocol::DataType::DATA_TYPE_KEYVALUE);
      request.__set_storage_type_(protocol::StorageType::type(protocol::StorageType::STORAGE_TYPE_CACHE |
          protocol::StorageType::STORAGE_TYPE_DISK));
      request.request_.assign(ptr, size);
      const char *ptr = this->packet_.Serialize<protocol::Request>(&request, size);
      if(ptr) {
        this->type_ = keyvalue.type_;
        this->key_ = key;
        this->Post(ptr, size);
        return true;
      } else {
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
            "%s:%d (%s) [KeyvalueTest] Serialize Request failed.",
            __FILE__, __LINE__, __FUNCTION__);
        return false;
      }
    } else {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [KeyvalueTest] Serialize KeyvalueRequest failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
  }

  global::ThriftPacket packet_;

  protocol::KeyvalueRequestType::type type_;
  std::string key_;
  std::string value_;

  KeyvalueRequestQueue requests_;

#ifdef _TIME_CONSUMING_TEST
  global::TimeConsuming time_consume_;
#endif
};

void TestSet(KeyvalueTest &test, int from, int to) {
  KeyvalueRequestConfigure request;
  std::ostringstream oss;
  for(int i = from; i < to; ++i) {
    oss.str("");
    oss << "key" << i;
    request.key_ = oss.str();
    oss.str("");
    oss << "value" << i;
    request.value_ = oss.str();
    request.type_ = protocol::KeyvalueRequestType::KEYVALUE_REQUEST_TYPE_SET;
    test.AddRequest(request);
  }
}

void TestGet(KeyvalueTest &test, int from, int to) {
  KeyvalueRequestConfigure request;
  std::ostringstream oss;
  for(int i = from; i < to; ++i) {
    oss.str("");
    oss << "key" << i;
    request.key_ = oss.str();
    oss.str("");
    oss << "value" << i;
    request.value_ = oss.str();
    request.type_ = protocol::KeyvalueRequestType::KEYVALUE_REQUEST_TYPE_GET;
    test.AddRequest(request);
  }
}

void TestDelete(KeyvalueTest &test, int from, int to) {
  KeyvalueRequestConfigure request;
  std::ostringstream oss;
  for(int i = from; i < to; ++i) {
    oss.str("");
    oss << "key" << i;
    request.key_ = oss.str();
    request.type_ = protocol::KeyvalueRequestType::KEYVALUE_REQUEST_TYPE_DELETE;
    test.AddRequest(request);
  }
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
    printf("[KeyvalueTest] Initialize EventLoop failed.\n");
    return 0;
  }

  KeyvalueTest test(core::InetAddress(host, port), "KeyvalueTest");
  if(test.Initialize(&loop) == false) {
    printf("[KeyvalueTest] Initialize failed.\n");
    return 0;
  }
  printf("[KeyvalueTest] Initialize\n");


  TestSet(test, 0, 99999);
  TestGet(test, 0, 99999);

  TestSet(test, 99999, 199999);
  TestGet(test, 99999, 199999);

  TestSet(test, 200000, 350000);
  TestGet(test, 200000, 350000);

  printf("[KeyvalueTest] Add request\n");

  test.Start();

  printf("[KeyvalueTest] Start\n");

  loop.Loop();

  global::AsyncLoggingSingleton::GetAsyncLogging()->Stop();

  return 0;
}

