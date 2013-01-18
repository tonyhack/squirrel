//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-11 17:53:19.
// File name: data_request_handler.cc
//
// Description: 
// Define class DataRequestHandler.
//

#include "gateway_server/data_request_handler.h"

#include "core/base/auto_mutex.h"
#include "common/keyvalue_message_types.h"
#include "common/message_data_proxy_types.h"
#include "common/list_message_types.h"
#include "common/number_message_types.h"
#include "data_proxy_server/client/data_proxy_client.h"
#include "gateway_server/http_context.h"
#include "gateway_server/http_response.h"
#include "global/async_logging_singleton.h"
#include "global/time_consuming.h"
#include "protocol/protocol_keyvalue_message_types.h"
#include "protocol/protocol_list_message_types.h"
#include "protocol/protocol_number_message_types.h"

namespace squirrel {

using namespace global;

namespace gateway {

#ifdef _TIME_CONSUMING_TEST
global::TimeConsuming total_consume_1("Dataproxyserver consuming time");
#endif

DataRequestHandler::DataRequestHandler() : buffer_(sizeof(core::uint64)) {
  this->map_request_handler_.Initialize(this);
}
DataRequestHandler::~DataRequestHandler() {}

bool DataRequestHandler::OnRequest(const protocol::Request &request,
    global::ThriftPacketPtr &packet, const core::TCPConnectionPtr &connection) {
#ifdef _TIME_CONSUMING_TEST
  total_consume_1.ResetConsuming();
#endif
  assert(connection);
  HttpContext *context = &boost::any_cast<HttpContext &>(connection->GetContext());
  if(context == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [DataRequestHandler] OnRequest HttpContext failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  switch(request.data_type_) {
    case protocol::DataType::DATA_TYPE_KEYVALUE:
      return this->OnKeyvalueRequest(request, packet, connection);
      break;
    case protocol::DataType::DATA_TYPE_NUMBER:
      return this->OnNumberRequest(request, packet, connection);
      break;
    case protocol::DataType::DATA_TYPE_LIST:
      return this->OnListRequest(request, packet, connection);
      break;
    case protocol::DataType::DATA_TYPE_MAP:
      return this->map_request_handler_.OnMapRequest(request, packet, connection);
      break;
    default:
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [DataRequestHandler] OnRequest Bad request type[%d].",
          __FILE__, __LINE__, __FUNCTION__, request.data_type_);
      return false;
      break;
  }
}

void DataRequestHandler::OnResponse(const common::MessageDataProxyDataResponse *response) {
#ifdef _TIME_CONSUMING_TEST
  total_consume_1.ConsumeLogging();
#endif
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
      "[DataRequestHandler] OnResponse request[%ld] key[%s] this=[%p]",
      response->request_id_, response->key_.c_str(), this);
  core::TCPConnectionPtr connection;
  if(this->RemoveResponse(response->request_id_, connection)) {
    switch(response->type_) {
      case common::RequestType::REQUEST_TYPE_KEYVALUE:
        this->OnKeyvalueResponse(connection, response->response_);
        break;
      case common::RequestType::REQUEST_TYPE_NUMBER:
        this->OnNumberResponse(connection, response->response_);
        break;
      case common::RequestType::REQUEST_TYPE_LIST:
        this->OnListResponse(connection, response->response_);
        break;
      case common::RequestType::REQUEST_TYPE_MAP:
        this->map_request_handler_.OnMapResponse(connection, response->response_);
        break;
      default:
        this->OnDefaultResponse(connection, response->response_);
        AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
            "%s:%d (%s) [DataRequestHandler] OnResponse Bad request type[%d].",
            __FILE__, __LINE__, __FUNCTION__, response->type_);
        break;
    }
    dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->DeallocateRequest(
        response->request_id_);
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [DataRequestHandler] OnResponse Bad request id[%ld].",
        __FILE__, __LINE__, __FUNCTION__, response->request_id_);
  }
}

bool DataRequestHandler::AddWaitResponse(const core::TCPConnectionPtr &connection,
    core::uint64 sequence) {
  HttpContext *context = &boost::any_cast<HttpContext &>(connection->GetContext());
  assert(context);
  context->SetRequestID(sequence);
  {
    core::AutoMutex auto_mutex(&this->mutex_);
    this->wait_responses_[sequence] = connection;
  }
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
      "[DataRequestHandler] [20120917] AddWaitResponse [%ld].", sequence);
  return true;
}

bool DataRequestHandler::RemoveResponse(core::uint64 sequence,
    core::TCPConnectionPtr &connection) {
  core::AutoMutex auto_mutex(&this->mutex_);
  WaitResponseMap::iterator iterator = this->wait_responses_.find(sequence);
  if(iterator != this->wait_responses_.end()) {
    connection = iterator->second;
    this->wait_responses_.erase(iterator);
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
        "[DataRequestHandler] [20120917] RemoveResponse [%ld].", sequence);
    return true;
  } 
  return false;
}

bool DataRequestHandler::OnKeyvalueRequest(const protocol::Request &request,
    global::ThriftPacketPtr &packet, const core::TCPConnectionPtr &connection) {
#ifdef _TIME_CONSUMING_TEST
  global::TimeConsuming time_consume("DataRequestHandler::OnKeyvalueRequest");
#endif
  assert(connection);
  protocol::KeyvalueRequest keyvalue;
  if(packet->Deserialize<protocol::KeyvalueRequest>(&keyvalue, request.request_) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [DataRequestHandler] Deserialize KeyvalueRequest failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  if(keyvalue.key_list_.empty()) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [DataRequestHandler] KeyvalueRequest empty key.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Keyvalue request.
  common::KeyvalueRequest common_keyvalue;
  common_keyvalue.__set_type_((common::KeyvalueRequestType::type)keyvalue.type_);
  if(keyvalue.type_ == protocol::KeyvalueRequestType::KEYVALUE_REQUEST_TYPE_SET) {
    if(keyvalue.value_list_.empty()) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [DataRequestHandler] KeyvalueRequest empty value.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    common_keyvalue.__set_value_(keyvalue.value_list_[0]);
  } else if(keyvalue.type_ >= protocol::KeyvalueRequestType::KEYVALUE_REQUEST_TYPE_MAX) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [DataRequestHandler] KeyvalueRequest type is error.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Serialize keyvalue request.
  size_t size = 0;
  const char *msg_ptr = packet->Serialize<common::KeyvalueRequest>(&common_keyvalue, size);
  if(msg_ptr == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [DataRequestHandler] Serialize KeyvalueRequest failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Get R/W type.
  common::RequestRWType::type rw_type;
  if(keyvalue.type_ == protocol::KeyvalueRequestType::KEYVALUE_REQUEST_TYPE_SET ||
      keyvalue.type_ == protocol::KeyvalueRequestType::KEYVALUE_REQUEST_TYPE_DELETE) {
    rw_type = common::RequestRWType::REQUEST_RW_TYPE_WRITE;
  } else {
    rw_type = common::RequestRWType::REQUEST_RW_TYPE_READ;
  }

  common::StorageType::type storage_type = (common::StorageType::type)request.storage_type_;

  // Allocate request id.
  core::uint64 sequence = 0;
  if(dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->AllocateRequest(
        sequence) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [DataRequestHandler] DataProxyClientSingleton allocate request failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Add response list.
  if(this->AddWaitResponse(connection, sequence) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [DataRequestHandler] DataProxyClientSingleton AddWaitResponse failed.",
        __FILE__, __LINE__, __FUNCTION__);
    dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->DeallocateRequest(
        sequence);
    return false;
  }

  // Request.
  if(dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->Request(
        common::RequestType::REQUEST_TYPE_KEYVALUE, storage_type, rw_type,
        keyvalue.key_list_[0], std::string(msg_ptr, size), sequence) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [DataRequestHandler] DataProxyClientSingleton request failed.",
        __FILE__, __LINE__, __FUNCTION__);
    dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->DeallocateRequest(
        sequence);
    core::TCPConnectionPtr connection_tmp;
    this->RemoveResponse(sequence, connection_tmp);
    return false;
  }

  return true;
}

void DataRequestHandler::OnKeyvalueResponse(const core::TCPConnectionPtr &connection,
    const std::string &res) {
#ifdef _TIME_CONSUMING_TEST
  global::TimeConsuming time_consume("DataRequestHandler::OnKeyvalueResponse");
#endif
  assert(connection);
  HttpResponse response;
  HttpContext *context = &boost::any_cast<HttpContext &>(connection->GetContext());
  if(context == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [GatewayServer] OnHttpRequest get http context failed.",
        __FILE__, __LINE__, __FUNCTION__);
    response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
    response.SetStatusMessage("Unknown error");
    response.SetCloseConnection(true);
  } else {
    common::KeyvalueResponse common_keyvalue;
    core::AutoMutex auto_mutex(&this->packet_mutex_);
    if(this->packet_.Deserialize<common::KeyvalueResponse>(&common_keyvalue, res) == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [DataRequestHandler] Deserialize KeyvalueResponse failed.",
          __FILE__, __LINE__, __FUNCTION__);
      response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
      response.SetStatusMessage("Bad Request");
      response.SetCloseConnection(true);
    } else {
      protocol::KeyvalueResponse protocol_keyvalue;
      protocol_keyvalue.__set_result_((protocol::KeyvalueResponseType::type)common_keyvalue.result_);
      if(common_keyvalue.__isset.value_ == true) {
        protocol_keyvalue.__isset.value_list_ = true;
        protocol_keyvalue.value_list_.push_back(common_keyvalue.value_);
      }
      size_t size = 0;
      const char *msg_ptr = this->packet_.Serialize<protocol::KeyvalueResponse>(&protocol_keyvalue, size);
      if(msg_ptr) {
        protocol::Response res;
        res.__set_data_type_(protocol::DataType::DATA_TYPE_KEYVALUE);
        res.response_.assign(msg_ptr, size);
        msg_ptr = this->packet_.Serialize<protocol::Response>(&res, size);
        if(msg_ptr) {
          response.SetStatusCode(HttpResponse::STATUS_200_OK);
          response.SetStatusMessage("OK");
          response.SetCloseConnection(false);
          response.SetContentType("thrift-0.8");
          response.AddHeader("Server", "Squirrel");
          response.SetBody(msg_ptr, size);
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
              "[DataRequestHandler] Http body size is %d", size);
        } else {
          response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
          response.SetStatusMessage("Serialize failed");
          response.SetCloseConnection(true);
        }
      } else {
        response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
        response.SetStatusMessage("Serialize failed");
        response.SetCloseConnection(true);
      }
    }
  }
  {
    core::AutoMutex auto_mutex(&this->packet_mutex_);
    this->buffer_.Reset();
    response.ToBuffer(&this->buffer_);
    connection->SendMessage(this->buffer_.ReadPointer(), this->buffer_.ReadSize());
  }
  if(response.CheckCloseConnection()) {
    if(context) context->GetRequest().SetCloseConnection();
  }
}

bool DataRequestHandler::OnNumberRequest(const protocol::Request &request,
    global::ThriftPacketPtr &packet, const core::TCPConnectionPtr &connection) {
#ifdef _TIME_CONSUMING_TEST
  global::TimeConsuming time_consume("DataRequestHandler::OnNumberRequest");
#endif
  assert(connection);
  protocol::NumberRequest number;
  if(packet->Deserialize<protocol::NumberRequest>(&number, request.request_) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [DataRequestHandler] Deserialize NumberRequest failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Number request.
  common::NumberRequest common_number;
  common_number.__set_type_((common::NumberRequestType::type)number.type_);
  if(number.type_ == protocol::NumberRequestType::NUMBER_REQUEST_TYPE_INC ||
      number.type_ == protocol::NumberRequestType::NUMBER_REQUEST_TYPE_DEC) {
    common_number.__set_increment_(number.increment_);
  } else if(number.type_ >= protocol::NumberRequestType::NUMBER_REQUEST_TYPE_MAX) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [DataRequestHandler] NumberRequest type is error.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Serialize number request.
  size_t size = 0;
  const char *msg_ptr = packet->Serialize<common::NumberRequest>(&common_number, size);
  if(msg_ptr == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [DataRequestHandler] Serialize NumberRequest failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Get R/W type.
  common::RequestRWType::type rw_type = common::RequestRWType::REQUEST_RW_TYPE_WRITE;
  common::StorageType::type storage_type = (common::StorageType::type)request.storage_type_;

  // Allocate request id.
  core::uint64 sequence = 0;
  if(dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->AllocateRequest(
        sequence) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [DataRequestHandler] DataProxyClientSingleton allocate request failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Add response list.
  if(this->AddWaitResponse(connection, sequence) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [DataRequestHandler] DataProxyClientSingleton AddWaitResponse failed.",
        __FILE__, __LINE__, __FUNCTION__);
    dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->DeallocateRequest(
        sequence);
    return false;
  }

  // Request.
  if(dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->Request(
        common::RequestType::REQUEST_TYPE_NUMBER, storage_type, rw_type,
        number.key_, std::string(msg_ptr, size), sequence) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [DataRequestHandler] DataProxyClientSingleton request failed.",
        __FILE__, __LINE__, __FUNCTION__);
    dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->DeallocateRequest(
        sequence);
    core::TCPConnectionPtr connection_tmp;
    this->RemoveResponse(sequence, connection_tmp);
    return false;
  }

  return true;
}

void DataRequestHandler::OnNumberResponse(const core::TCPConnectionPtr &connection,
    const std::string &res) {
#ifdef _TIME_CONSUMING_TEST
  global::TimeConsuming time_consume("DataRequestHandler::OnNumberResponse");
#endif
  assert(connection);
  HttpResponse response;
  HttpContext *context = &boost::any_cast<HttpContext &>(connection->GetContext());
  if(context == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [GatewayServer] OnHttpRequest get http context failed.",
        __FILE__, __LINE__, __FUNCTION__);
    response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
    response.SetStatusMessage("Unknown error");
    response.SetCloseConnection(true);
  } else {
    common::NumberResponse common_number;
    core::AutoMutex auto_mutex(&this->packet_mutex_);
    if(this->packet_.Deserialize<common::NumberResponse>(&common_number, res) == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [DataRequestHandler] Deserialize NumberResponse failed.",
          __FILE__, __LINE__, __FUNCTION__);
      response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
      response.SetStatusMessage("Bad Request");
      response.SetCloseConnection(true);
    } else {
      protocol::NumberResponse protocol_number;
      protocol_number.__set_result_((protocol::NumberResponseType::type)common_number.result_);
      if(common_number.__isset.value_ == true) {
        protocol_number.__set_value_(common_number.value_);
      }
      size_t size = 0;
      const char *msg_ptr = this->packet_.Serialize<protocol::NumberResponse>(&protocol_number, size);
      if(msg_ptr) {
        protocol::Response res;
        res.__set_data_type_(protocol::DataType::DATA_TYPE_NUMBER);
        res.response_.assign(msg_ptr, size);
        msg_ptr = this->packet_.Serialize<protocol::Response>(&res, size);
        if(msg_ptr) {
          response.SetStatusCode(HttpResponse::STATUS_200_OK);
          response.SetStatusMessage("OK");
          response.SetCloseConnection(false);
          response.SetContentType("thrift-0.8");
          response.AddHeader("Server", "Squirrel");
          response.SetBody(msg_ptr, size);
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
              "[DataRequestHandler] Http body size is %d", size);
        } else {
          response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
          response.SetStatusMessage("Serialize failed");
          response.SetCloseConnection(true);
        }
      } else {
        response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
        response.SetStatusMessage("Serialize failed");
        response.SetCloseConnection(true);
      }
    }
  }
  {
    core::AutoMutex auto_mutex(&this->packet_mutex_);
    this->buffer_.Reset();
    response.ToBuffer(&this->buffer_);
    connection->SendMessage(this->buffer_.ReadPointer(), this->buffer_.ReadSize());
  }
  if(response.CheckCloseConnection()) {
    if(context) context->GetRequest().SetCloseConnection();
  }
}

bool DataRequestHandler::OnListRequest(const protocol::Request &request,
    global::ThriftPacketPtr &packet, const core::TCPConnectionPtr &connection) {
#ifdef _TIME_CONSUMING_TEST
  global::TimeConsuming time_consume("DataRequestHandler::OnListRequest");
#endif
  assert(connection);
  protocol::ListRequest list;
  if(packet->Deserialize<protocol::ListRequest>(&list, request.request_) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [DataRequestHandler] Deserialize ListRequest failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // List request.
  common::ListRequest common_list;
  common_list.__set_type_((common::ListRequestType::type)list.type_);
  if(list.type_ == protocol::ListRequestType::LIST_REQUEST_TYPE_PUSH ||
      list.type_ == protocol::ListRequestType::LIST_REQUEST_TYPE_REMOVE) {
    common_list.__set_data_(list.data_);
  } else if(list.type_ >= protocol::ListRequestType::LIST_REQUEST_TYPE_MAX) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [DataRequestHandler] ListRequest type is error.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Serialize list request.
  size_t size = 0;
  const char *msg_ptr = packet->Serialize<common::ListRequest>(&common_list, size);
  if(msg_ptr == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [DataRequestHandler] Serialize ListRequest failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Get R/W type.
  common::RequestRWType::type rw_type = common::RequestRWType::REQUEST_RW_TYPE_WRITE;
  common::StorageType::type storage_type = (common::StorageType::type)request.storage_type_;

  // Allocate request id.
  core::uint64 sequence = 0;
  if(dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->AllocateRequest(
        sequence) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [DataRequestHandler] DataProxyClientSingleton allocate request failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Add response list.
  if(this->AddWaitResponse(connection, sequence) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [DataRequestHandler] DataProxyClientSingleton AddWaitResponse failed.",
        __FILE__, __LINE__, __FUNCTION__);
    dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->DeallocateRequest(
        sequence);
    return false;
  }

  // Request.
  if(dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->Request(
        common::RequestType::REQUEST_TYPE_LIST, storage_type, rw_type,
        list.key_, std::string(msg_ptr, size), sequence) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [DataRequestHandler] DataProxyClientSingleton request failed.",
        __FILE__, __LINE__, __FUNCTION__);
    dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->DeallocateRequest(
        sequence);
    core::TCPConnectionPtr connection_tmp;
    this->RemoveResponse(sequence, connection_tmp);
    return false;
  }

  return true;
}

void DataRequestHandler::OnListResponse(const core::TCPConnectionPtr &connection,
    const std::string &res) {
#ifdef _TIME_CONSUMING_TEST
  global::TimeConsuming time_consume("DataRequestHandler::OnListResponse");
#endif
  assert(connection);
  HttpResponse response;
  HttpContext *context = &boost::any_cast<HttpContext &>(connection->GetContext());
  if(context == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [GatewayServer] OnHttpRequest get http context failed.",
        __FILE__, __LINE__, __FUNCTION__);
    response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
    response.SetStatusMessage("Unknown error");
    response.SetCloseConnection(true);
  } else {
    common::ListResponse common_list;
    core::AutoMutex auto_mutex(&this->packet_mutex_);
    if(this->packet_.Deserialize<common::ListResponse>(&common_list, res) == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [DataRequestHandler] Deserialize ListResponse failed.",
          __FILE__, __LINE__, __FUNCTION__);
      response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
      response.SetStatusMessage("Bad Request");
      response.SetCloseConnection(true);
    } else {
      protocol::ListResponse protocol_list;
      protocol_list.__set_result_((protocol::ListResponseType::type)common_list.result_);
      if(common_list.__isset.data_ == true) {
        protocol_list.__set_data_(common_list.data_);
      }
      size_t size = 0;
      const char *msg_ptr = this->packet_.Serialize<protocol::ListResponse>(&protocol_list, size);
      if(msg_ptr) {
        protocol::Response res;
        res.__set_data_type_(protocol::DataType::DATA_TYPE_LIST);
        res.response_.assign(msg_ptr, size);
        msg_ptr = this->packet_.Serialize<protocol::Response>(&res, size);
        if(msg_ptr) {
          response.SetStatusCode(HttpResponse::STATUS_200_OK);
          response.SetStatusMessage("OK");
          response.SetCloseConnection(false);
          response.SetContentType("thrift-0.8");
          response.AddHeader("Server", "Squirrel");
          response.SetBody(msg_ptr, size);
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
              "[DataRequestHandler] Http body size is %d", size);
        } else {
          response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
          response.SetStatusMessage("Serialize failed");
          response.SetCloseConnection(true);
        }
      } else {
        response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
        response.SetStatusMessage("Serialize failed");
        response.SetCloseConnection(true);
      }
    }
  }
  {
    core::AutoMutex auto_mutex(&this->packet_mutex_);
    this->buffer_.Reset();
    response.ToBuffer(&this->buffer_);
    connection->SendMessage(this->buffer_.ReadPointer(), this->buffer_.ReadSize());
  }
  if(response.CheckCloseConnection()) {
    if(context) context->GetRequest().SetCloseConnection();
  }
}

void DataRequestHandler::OnDefaultResponse(const core::TCPConnectionPtr &connection,
    const std::string &res) {
  HttpResponse response;
  response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
  response.SetStatusMessage("Serialize failed");
  response.SetCloseConnection(true);
  {
    core::AutoMutex auto_mutex(&this->packet_mutex_);
    this->buffer_.Reset();
    response.ToBuffer(&this->buffer_);
    connection->SendMessage(this->buffer_.ReadPointer(), this->buffer_.ReadSize());
  }
  if(response.CheckCloseConnection()) {
    HttpContext *context = &boost::any_cast<HttpContext &>(connection->GetContext());
    if(context) context->GetRequest().SetCloseConnection();
  }
}

}  // namespace gateway

}  // namespace squirrel

