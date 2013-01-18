//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-20 13:51:09.
// File name: number_test.cc
//
// Description: 
// Test of number.
//

#include <deque>
#include <sstream>

#include "core/configure.h"
#include "core/event_loop.h"
#include "protocol/message_types.h"
#include "protocol/protocol_number_message_types.h"
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

struct NumberRequestConfigure {
  std::string key_;
  std::string increment_;
  protocol::NumberRequestType::type type_;
  protocol::StorageType::type storage_type_;
};

class NumberTest : public HttpClient {
  typedef std::deque<NumberRequestConfigure> NumberRequestQueue;
 public:
  NumberTest(const core::InetAddress &server_address,
      const std::string &name) : HttpClient(server_address, name) {
#ifdef _TIME_CONSUMING_TEST
    time_consume_.SetDebugInfo("consumer test");
#endif
  }

  virtual ~NumberTest() {}

  virtual bool OnMessage(const char *message, size_t size) {
#ifdef _TIME_CONSUMING_TEST
    this->time_consume_.ConsumeLogging();
#endif
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
        "[NumberTest] OnMessage.");
    // Response.
    protocol::Response response;
    if(this->packet_.Deserialize<protocol::Response>(&response, message, size) == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [NumberTest] Deserialize Request failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    if(response.data_type_ != protocol::DataType::DATA_TYPE_NUMBER) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [NumberTest] Response not DATA_TYPE_NUMBER type.",
          __FILE__, __LINE__, __FUNCTION__);
        return false;
    }

    // NumberRequest.
    protocol::NumberResponse number;
    if(this->packet_.Deserialize<protocol::NumberResponse>(&number, response.response_) == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [NumberTest] Deserialize NumberRequest failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    if(number.result_ != protocol::NumberResponseType::NUMBER_RESPONSE_TYPE_SUCCESS) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [NumberTest] number(type=%d) response failed(%d).",
          __FILE__, __LINE__, __FUNCTION__, this->type_, number.result_);
      return false;
    }

    switch(this->type_) {
      case protocol::NumberRequestType::NUMBER_REQUEST_TYPE_INC:
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
            "[NumberTest] inc(%s, %s) response(%s)", this->key_.c_str(),
            this->increment_.c_str(), number.value_.c_str());
        break;

      case protocol::NumberRequestType::NUMBER_REQUEST_TYPE_DEC:
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
            "[NumberTest] dec(%s, %s) response(%s)", this->key_.c_str(),
            this->increment_.c_str(), number.value_.c_str());
        break;

      case protocol::NumberRequestType::NUMBER_REQUEST_TYPE_RESET:
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
            "[NumberTest] reset(%s) response", this->key_.c_str());
        break;

      case protocol::NumberRequestType::NUMBER_REQUEST_TYPE_DELETE:
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
            "[NumberTest] delete(%s) response", this->key_.c_str());
        break;

      default:
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
            "[NumberTest] Response error type(%d)", this->type_);
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

  inline void AddRequest(const NumberRequestConfigure &request) {
    this->requests_.push_back(request);
  }

 private:
  void DoRequest() {
    if(this->requests_.empty() == false) {
      NumberRequestConfigure request = this->requests_.front();
      this->requests_.pop_front();
      switch(request.type_) {
        case protocol::NumberRequestType::NUMBER_REQUEST_TYPE_INC:
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
              "[NumberTest] inc(%s, %s) request", request.key_.c_str(),
              request.increment_.c_str());
          this->TestInc(request.key_, request.increment_, request.storage_type_);
          break;
        case protocol::NumberRequestType::NUMBER_REQUEST_TYPE_DEC:
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
              "[NumberTest] dec(%s, %s) request", request.key_.c_str(),
              request.increment_.c_str());
          this->TestDec(request.key_, request.increment_, request.storage_type_);
          break;
          break;
        case protocol::NumberRequestType::NUMBER_REQUEST_TYPE_RESET:
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
              "[NumberTest] reset(%s) request", request.key_.c_str());
          this->TestReset(request.key_, request.storage_type_);
          break;
        case protocol::NumberRequestType::NUMBER_REQUEST_TYPE_DELETE:
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
              "[NumberTest] delete(%s) request", request.key_.c_str());
          this->TestDelete(request.key_, request.storage_type_);
          break;
        default:
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
              "%s:%d (%s) [NumberTest] Error request type(%d).",
              __FILE__, __LINE__, __FUNCTION__, request.type_);
          break;
      }
    }
#ifdef _TIME_CONSUMING_TEST
    this->time_consume_.ResetConsuming();
#endif
  }

  bool TestInc(const std::string &key, const std::string &increment,
      protocol::StorageType::type storage_type) {
    protocol::NumberRequest number;
    number.__set_type_(protocol::NumberRequestType::NUMBER_REQUEST_TYPE_INC);
    number.__set_key_(key);
    number.__set_increment_(increment);
    size_t size = 0;
    const char *ptr = this->packet_.Serialize<protocol::NumberRequest>(&number, size);
    if(ptr) {
      protocol::Request request;
      request.__set_data_type_(protocol::DataType::DATA_TYPE_NUMBER);
      request.__set_storage_type_(storage_type);
      request.request_.assign(ptr, size);
      ptr = this->packet_.Serialize<protocol::Request>(&request, size);
      if(ptr) {
        this->type_ = number.type_;
        this->key_ = key;
        this->increment_ = increment;
        this->Post(ptr, size);
        return true;
      } else {
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
            "%s:%d (%s) [NumberTest] Serialize Request failed.",
            __FILE__, __LINE__, __FUNCTION__);
        return false;
      }
    } else {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [NumberTest] Serialize NumberRequest failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
  }

  bool TestDec(const std::string &key, const std::string &decrement,
        protocol::StorageType::type storage_type) {
    protocol::NumberRequest number;
    number.__set_type_(protocol::NumberRequestType::NUMBER_REQUEST_TYPE_DEC);
    number.__set_key_(key);
    number.__set_increment_(decrement);
    size_t size = 0;
    const char *ptr = this->packet_.Serialize<protocol::NumberRequest>(&number, size);
    if(ptr) {
      protocol::Request request;
      request.__set_data_type_(protocol::DataType::DATA_TYPE_NUMBER);
      request.__set_storage_type_(storage_type);
      request.request_.assign(ptr, size);
      ptr = this->packet_.Serialize<protocol::Request>(&request, size);
      if(ptr) {
        this->type_ = number.type_;
        this->key_ = key;
        this->increment_ = decrement;
        this->Post(ptr, size);
        return true;
      } else {
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
            "%s:%d (%s) [NumberTest] Serialize Request failed.",
            __FILE__, __LINE__, __FUNCTION__);
        return false;
      }
    } else {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [NumberTest] Serialize NumberRequest failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
  }

  bool TestReset(const std::string &key, protocol::StorageType::type storage_type) {
    protocol::NumberRequest number;
    number.__set_type_(protocol::NumberRequestType::NUMBER_REQUEST_TYPE_RESET);
    number.__set_key_(key);
    size_t size = 0;
    const char *ptr = this->packet_.Serialize<protocol::NumberRequest>(&number, size);
    if(ptr) {
      protocol::Request request;
      request.__set_data_type_(protocol::DataType::DATA_TYPE_NUMBER);
      request.__set_storage_type_(storage_type);
      request.request_.assign(ptr, size);
      ptr = this->packet_.Serialize<protocol::Request>(&request, size);
      if(ptr) {
        this->type_ = number.type_;
        this->key_ = key;
        this->Post(ptr, size);
        return true;
      } else {
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
            "%s:%d (%s) [NumberTest] Serialize Request failed.",
            __FILE__, __LINE__, __FUNCTION__);
        return false;
      }
    } else {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [NumberTest] Serialize NumberRequest failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
  }

  bool TestDelete(const std::string &key, protocol::StorageType::type storage_type) {
    protocol::NumberRequest number;
    number.__set_type_(protocol::NumberRequestType::NUMBER_REQUEST_TYPE_DELETE);
    number.__set_key_(key);
    size_t size = 0;
    const char *ptr = this->packet_.Serialize<protocol::NumberRequest>(&number, size);
    if(ptr) {
      protocol::Request request;
      request.__set_data_type_(protocol::DataType::DATA_TYPE_NUMBER);
      request.__set_storage_type_(storage_type);
      request.request_.assign(ptr, size);
      ptr = this->packet_.Serialize<protocol::Request>(&request, size);
      if(ptr) {
        this->type_ = number.type_;
        this->key_ = key;
        this->Post(ptr, size);
        return true;
      } else {
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
            "%s:%d (%s) [NumberTest] Serialize Request failed.",
            __FILE__, __LINE__, __FUNCTION__);
        return false;
      }
    } else {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [NumberTest] Serialize NumberRequest failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
  }

  global::ThriftPacket packet_;

  protocol::NumberRequestType::type type_;
  std::string key_;
  std::string increment_;

  NumberRequestQueue requests_;

#ifdef _TIME_CONSUMING_TEST
  global::TimeConsuming time_consume_;
#endif
};

void TestInc(NumberTest &test, int from, int to, bool redis = true) {
  NumberRequestConfigure request;
  std::ostringstream oss;
  for(int i = from; i < to; ++i) {
    oss.str("");
    oss << "Number003-" << i;
    request.key_ = oss.str();
    oss.str("");
    oss << i;
    request.increment_ = oss.str();
    request.type_ = protocol::NumberRequestType::NUMBER_REQUEST_TYPE_INC;
    request.storage_type_ = redis ? protocol::StorageType::STORAGE_TYPE_CACHE :
      protocol::StorageType::STORAGE_TYPE_DISK;
    test.AddRequest(request);
  }
}

void TestDec(NumberTest &test, int from, int to, bool redis = true) {
  NumberRequestConfigure request;
  std::ostringstream oss;
  for(int i = from; i < to; ++i) {
    oss.str("");
    oss << "Number003-" << i;
    request.key_ = oss.str();
    oss.str("");
    oss << i;
    request.increment_ = oss.str();
    request.type_ = protocol::NumberRequestType::NUMBER_REQUEST_TYPE_DEC;
    request.storage_type_ = redis ? protocol::StorageType::STORAGE_TYPE_CACHE :
      protocol::StorageType::STORAGE_TYPE_DISK;
    test.AddRequest(request);
  }
}

void TestReset(NumberTest &test, int from, int to, bool redis = true) {
  NumberRequestConfigure request;
  std::ostringstream oss;
  for(int i = from; i < to; ++i) {
    oss.str("");
    oss << "Number003-" << i;
    request.key_ = oss.str();
    request.type_ = protocol::NumberRequestType::NUMBER_REQUEST_TYPE_RESET;
    request.storage_type_ = redis ? protocol::StorageType::STORAGE_TYPE_CACHE :
      protocol::StorageType::STORAGE_TYPE_DISK;
    test.AddRequest(request);
  }
}

void TestDelete(NumberTest &test, int from, int to, bool redis = true) {
  NumberRequestConfigure request;
  std::ostringstream oss;
  for(int i = from; i < to; ++i) {
    oss.str("");
    oss << "Number003-" << i;
    request.key_ = oss.str();
    request.type_ = protocol::NumberRequestType::NUMBER_REQUEST_TYPE_DELETE;
    request.storage_type_ = redis ? protocol::StorageType::STORAGE_TYPE_CACHE :
      protocol::StorageType::STORAGE_TYPE_DISK;
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
    printf("[NumberTest] Initialize EventLoop failed.\n");
    return 0;
  }

  NumberTest test(core::InetAddress(host, port), "NumberTest");
  if(test.Initialize(&loop) == false) {
    printf("[NumberTest] Initialize failed.\n");
    return 0;
  }
  printf("[NumberTest] Initialize\n");


  // redis test.
  TestInc(test, 0, 99);
  TestDec(test, 0, 99);

  TestReset(test, 0, 99);

  TestInc(test, 0, 99);
  TestDec(test, 0, 99);

  TestDelete(test, 0, 99);


  // storage test.
  TestInc(test, 0, 99, false);
  TestDec(test, 0, 99, false);

  TestReset(test, 0, 99, false);

  TestInc(test, 0, 99, false);
  TestDec(test, 0, 99, false);

  TestDelete(test, 0, 99, false);


  printf("[KeyvalueTest] Add request\n");

  test.Start();

  printf("[KeyvalueTest] Start\n");

  loop.Loop();

  global::AsyncLoggingSingleton::GetAsyncLogging()->Stop();

  return 0;
}

