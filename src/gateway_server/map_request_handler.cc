//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-28 16:23:08.
// File name: map_request_handler.cc
//
// Description: 
// Define class MapRequestHandler.
//

#include "gateway_server/map_request_handler.h"

#include "core/base/auto_mutex.h"
#include "common/map_message_types.h"
#include "common/message_data_proxy_types.h"
#include "data_proxy_server/client/data_proxy_client.h"
#include "gateway_server/data_request_handler.h"
#include "gateway_server/http_context.h"
#include "gateway_server/http_response.h"
#include "global/async_logging_singleton.h"
#include "global/time_consuming.h"
#include "protocol/protocol_map_message_types.h"


namespace squirrel {

namespace gateway {

MapRequestHandler::MapRequestHandler()
  : data_request_handler_(NULL), buffer_(sizeof(core::uint64)) {}
MapRequestHandler::~MapRequestHandler() {}

bool MapRequestHandler::OnMapRequest(const protocol::Request &request,
    global::ThriftPacketPtr &packet, const core::TCPConnectionPtr &connection) {
#ifdef _TIME_CONSUMING_TEST
  global::TimeConsuming time_consume("MapRequestHandler::OnMapRequest");
#endif
  assert(connection);
  protocol::MapRequest map;
  if(packet->Deserialize<protocol::MapRequest>(&map, request.request_) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Deserialize MapRequest failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  switch(map.type_) {
    case protocol::MapRequestType::MAP_REQUEST_TYPE_INSERT:
      return this->OnRequestInsert(map, packet, connection,
          (common::StorageType::type)request.storage_type_);
    case protocol::MapRequestType::MAP_REQUEST_TYPE_REMOVE:
      return this->OnRequestRemove(map, packet, connection,
          (common::StorageType::type)request.storage_type_);
    case protocol::MapRequestType::MAP_REQUEST_TYPE_QUERY_RANKING:
      return this->OnRequestQueryRank(map, packet, connection,
          (common::StorageType::type)request.storage_type_);
    case protocol::MapRequestType::MAP_REQUEST_TYPE_QUERY_RANKING_RANGE:
      return this->OnRequestQueryRange(map, packet, connection,
          (common::StorageType::type)request.storage_type_);
    case protocol::MapRequestType::MAP_REQUEST_TYPE_RESET_RANKING_KEY:
       return this->OnRequestResetRankingKey(map, packet, connection,
          (common::StorageType::type)request.storage_type_);   
    case protocol::MapRequestType::MAP_REQUEST_TYPE_CLEAR:
      return this->OnRequestClear(map, packet, connection,
          (common::StorageType::type)request.storage_type_);
    case protocol::MapRequestType::MAP_REQUEST_TYPE_DELETE:
      return this->OnRequestDelete(map, packet, connection,
          (common::StorageType::type)request.storage_type_);
    default:
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [MapRequestHandler] Error map request type[%d].",
          __FILE__, __LINE__, __FUNCTION__, map.type_);
      return false;
  }
  return true;
}

void MapRequestHandler::OnMapResponse(const core::TCPConnectionPtr &connection,
    const std::string &res) {
#ifdef _TIME_CONSUMING_TEST
  global::TimeConsuming time_consume("MapRequestHandler::OnMapResponse");
#endif
  assert(connection);
  HttpResponse response;
  HttpContext *context = &boost::any_cast<HttpContext &>(connection->GetContext());
  if(context == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] OnHttpRequest get http context failed.",
        __FILE__, __LINE__, __FUNCTION__);
    response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
    response.SetStatusMessage("Unknown error");
    response.SetCloseConnection(true);
  } else {
    common::MapResponse common_map;
    core::AutoMutex auto_mutex(&this->packet_mutex_);
    if(this->packet_.Deserialize<common::MapResponse>(&common_map, res) == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [MapRequestHandler] Deserialize MapResponse failed.",
          __FILE__, __LINE__, __FUNCTION__);
      response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
      response.SetStatusMessage("Bad Request");
      response.SetCloseConnection(true);
    } else {
      switch(common_map.type_) {
        case protocol::MapRequestType::MAP_REQUEST_TYPE_INSERT:
          this->OnResponseInsert(common_map, response);
          break;
        case protocol::MapRequestType::MAP_REQUEST_TYPE_REMOVE:
          this->OnResponseRemove(common_map, response);
          break;
        case protocol::MapRequestType::MAP_REQUEST_TYPE_QUERY_RANKING:
          this->OnResponseQueryRank(common_map, response);
          break;
        case protocol::MapRequestType::MAP_REQUEST_TYPE_QUERY_RANKING_RANGE:
          this->OnResponseQueryRange(common_map, response);
          break;
        case protocol::MapRequestType::MAP_REQUEST_TYPE_RESET_RANKING_KEY:
          this->OnResponseResetRankingKey(common_map, response);
          break;
        case protocol::MapRequestType::MAP_REQUEST_TYPE_CLEAR:
          this->OnResponseClear(common_map, response);
          break;
        case protocol::MapRequestType::MAP_REQUEST_TYPE_DELETE:
          this->OnResponseDelete(common_map, response);
          break;
        default:
          AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
              "%s:%d (%s) [MapRequestHandler] Error map request type[%d].",
              __FILE__, __LINE__, __FUNCTION__, common_map.type_);
          response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
          response.SetStatusMessage("Request type error");
          response.SetCloseConnection(true);
          break;
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

bool MapRequestHandler::OnRequestInsert(const protocol::MapRequest &request,
    global::ThriftPacketPtr &packet, const core::TCPConnectionPtr &connection,
    common::StorageType::type storage_type) {
  // Deserialize insert request.
  protocol::MapInsertRequest insert;
  if(packet->Deserialize<protocol::MapInsertRequest>(&insert, request.data_) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Deserialize MapInsertRequest failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Common insert reqeust.
  common::MapInsertRequest common_insert;
  common_insert.__set_ranking_key_(insert.ranking_key_);
  common_insert.__set_element_(insert.element_);
  size_t size = 0;
  const char *ptr = packet->Serialize<common::MapInsertRequest>(&common_insert, size);
  if(ptr == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Serialize MapInsertRequest failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Common request
  common::MapRequest common_request;
  common_request.__set_type_(common::MapRequestType::MAP_REQUEST_TYPE_INSERT);
  common_request.__isset.data_ = true;
  common_request.data_.assign(ptr, size);
  common_request.__set_key_(request.key_);
  ptr = packet->Serialize<common::MapRequest>(&common_request, size);
  if(ptr == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Serialize MapRequest failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Allocate request id.
  core::uint64 sequence = 0;
  if(dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->AllocateRequest(
        sequence) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] DataProxyClientSingleton allocate request failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Add response list.
  if(this->data_request_handler_->AddWaitResponse(connection, sequence) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] AddWaitResponse failed.",
        __FILE__, __LINE__, __FUNCTION__);
    dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->DeallocateRequest(
        sequence);
    return false;
  }

  // RD type.
  common::RequestRWType::type rd_type = common::RequestRWType::REQUEST_RW_TYPE_WRITE;

  // Request.
  if(dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->Request(
        common::RequestType::REQUEST_TYPE_MAP, storage_type, rd_type,
        request.key_, std::string(ptr, size), sequence) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] DataProxyClientSingleton Request failed.",
        __FILE__, __LINE__, __FUNCTION__);
    dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->DeallocateRequest(
        sequence);
    core::TCPConnectionPtr connection_tmp;
    this->data_request_handler_->RemoveResponse(sequence, connection_tmp);
    return false;
  }

  return true;
}

void MapRequestHandler::OnResponseInsert(const common::MapResponse &map_response,
    HttpResponse &response) {
  // Protocol map response.
  protocol::MapResponse res;
  res.__set_type_((protocol::MapResponseType::type)map_response.result_);
  size_t size = 0;
  const char *ptr = this->packet_.Serialize<protocol::MapResponse>(&res, size);
  if(ptr == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Serialize MapResponse failed.",
        __FILE__, __LINE__, __FUNCTION__);
    response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
    response.SetStatusMessage("Serialize failed");
    response.SetCloseConnection(true);
    return ;
  }

  // Protocol response.
  protocol::Response message;
  message.__set_data_type_(protocol::DataType::DATA_TYPE_MAP);
  message.response_.assign(ptr, size);
  ptr = this->packet_.Serialize<protocol::Response>(&message, size);
  if(ptr == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Serialize Response failed.",
        __FILE__, __LINE__, __FUNCTION__);
    response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
    response.SetStatusMessage("Serialize failed");
    response.SetCloseConnection(true);
    return ;
  }

  // Http response.
  response.SetStatusCode(HttpResponse::STATUS_200_OK);
  response.SetStatusMessage("OK");
  response.SetCloseConnection(false);
  response.SetContentType("thrift-0.8");
  response.AddHeader("Server", "Squirrel");
  response.SetBody(ptr, size);
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
      "[MapRequestHandler] Http body size is %d", size);
}

bool MapRequestHandler::OnRequestRemove(const protocol::MapRequest &request,
    global::ThriftPacketPtr &packet, const core::TCPConnectionPtr &connection,
    common::StorageType::type storage_type) {
  // Deserialize remove request.
  protocol::MapRemoveRequest remove;
  if(packet->Deserialize<protocol::MapRemoveRequest>(&remove, request.data_) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Deserialize MapRemoveRequest failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Common remove reqeust.
  common::MapRemoveRequest common_remove;
  common_remove.__set_element_(remove.element_);
  size_t size = 0;
  const char *ptr = packet->Serialize<common::MapRemoveRequest>(&common_remove, size);
  if(ptr == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Serialize MapRemoveRequest failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Common request
  common::MapRequest common_request;
  common_request.__set_type_(common::MapRequestType::MAP_REQUEST_TYPE_REMOVE);
  common_request.__isset.data_ = true;
  common_request.data_.assign(ptr, size);
  common_request.__set_key_(request.key_);
  ptr = packet->Serialize<common::MapRequest>(&common_request, size);
  if(ptr == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Serialize MapRequest failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Allocate request id.
  core::uint64 sequence = 0;
  if(dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->AllocateRequest(
        sequence) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] DataProxyClientSingleton allocate request failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Add response list.
  if(this->data_request_handler_->AddWaitResponse(connection, sequence) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] AddWaitResponse failed.",
        __FILE__, __LINE__, __FUNCTION__);
    dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->DeallocateRequest(
        sequence);
    return false;
  }

  // RD type.
  common::RequestRWType::type rd_type = common::RequestRWType::REQUEST_RW_TYPE_WRITE;

  // Request.
  if(dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->Request(
        common::RequestType::REQUEST_TYPE_MAP, storage_type, rd_type,
        request.key_, std::string(ptr, size), sequence) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] DataProxyClientSingleton Request failed.",
        __FILE__, __LINE__, __FUNCTION__);
    dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->DeallocateRequest(
        sequence);
    core::TCPConnectionPtr connection_tmp;
    this->data_request_handler_->RemoveResponse(sequence, connection_tmp);
    return false;
  }

  return true;
}

void MapRequestHandler::OnResponseRemove(const common::MapResponse &map_response,
    HttpResponse &response) {
  // Protocol map response.
  protocol::MapResponse res;
  res.__set_type_((protocol::MapResponseType::type)map_response.result_);
  size_t size = 0;
  const char *ptr = this->packet_.Serialize<protocol::MapResponse>(&res, size);
  if(ptr == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Serialize MapResponse failed.",
        __FILE__, __LINE__, __FUNCTION__);
    response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
    response.SetStatusMessage("Serialize failed");
    response.SetCloseConnection(true);
    return ;
  }

  // Protocol response.
  protocol::Response message;
  message.__set_data_type_(protocol::DataType::DATA_TYPE_MAP);
  message.response_.assign(ptr, size);
  ptr = this->packet_.Serialize<protocol::Response>(&message, size);
  if(ptr == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Serialize Response failed.",
        __FILE__, __LINE__, __FUNCTION__);
    response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
    response.SetStatusMessage("Serialize failed");
    response.SetCloseConnection(true);
    return ;
  }

  // Http response.
  response.SetStatusCode(HttpResponse::STATUS_200_OK);
  response.SetStatusMessage("OK");
  response.SetCloseConnection(false);
  response.SetContentType("thrift-0.8");
  response.AddHeader("Server", "Squirrel");
  response.SetBody(ptr, size);
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
      "[MapRequestHandler] Http body size is %d", size);
}

bool MapRequestHandler::OnRequestQueryRank(const protocol::MapRequest &request,
    global::ThriftPacketPtr &packet, const core::TCPConnectionPtr &connection,
    common::StorageType::type storage_type) {
  // Deserialize query request.
  protocol::MapQueryRankingRequest query;
  if(packet->Deserialize<protocol::MapQueryRankingRequest>(&query, request.data_) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Deserialize MapQueryRankingRequest failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Common query reqeust.
  common::MapQueryRankingRequest common_query;
  common_query.__set_element_(query.element_);
  common_query.__set_sort_type_((common::MapSortType::type)query.sort_type_);
  size_t size = 0;
  const char *ptr = packet->Serialize<common::MapQueryRankingRequest>(&common_query, size);
  if(ptr == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Serialize MapQueryRankingRequest failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Common request
  common::MapRequest common_request;
  common_request.__set_type_(common::MapRequestType::MAP_REQUEST_TYPE_QUERY_RANKING);
  common_request.__isset.data_ = true;
  common_request.data_.assign(ptr, size);
  common_request.__set_key_(request.key_);
  ptr = packet->Serialize<common::MapRequest>(&common_request, size);
  if(ptr == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Serialize MapRequest failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Allocate request id.
  core::uint64 sequence = 0;
  if(dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->AllocateRequest(
        sequence) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] DataProxyClientSingleton allocate request failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Add response list.
  if(this->data_request_handler_->AddWaitResponse(connection, sequence) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] AddWaitResponse failed.",
        __FILE__, __LINE__, __FUNCTION__);
    dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->DeallocateRequest(
        sequence);
    return false;
  }

  // RD type.
  common::RequestRWType::type rd_type = common::RequestRWType::REQUEST_RW_TYPE_READ;

  // Request.
  if(dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->Request(
        common::RequestType::REQUEST_TYPE_MAP, storage_type, rd_type,
        request.key_, std::string(ptr, size), sequence) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] DataProxyClientSingleton Request failed.",
        __FILE__, __LINE__, __FUNCTION__);
    dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->DeallocateRequest(
        sequence);
    core::TCPConnectionPtr connection_tmp;
    this->data_request_handler_->RemoveResponse(sequence, connection_tmp);
    return false;
  }

  return true;
}

void MapRequestHandler::OnResponseQueryRank(const common::MapResponse &map_response,
    HttpResponse &response) {
  // Protocol map response.
  protocol::MapResponse res;
  res.__set_type_((protocol::MapResponseType::type)map_response.result_);

  if(map_response.result_ == common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS) {
    // Deserialize MapQueryRankingResponse.
    common::MapQueryRankingResponse ranking_response;
    if(this->packet_.Deserialize<common::MapQueryRankingResponse>(&ranking_response,
          map_response.data_.data(), map_response.data_.size()) == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [MapRequestHandler] Deserialize MapQueryRankingRequest failed.",
          __FILE__, __LINE__, __FUNCTION__);
      response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
      response.SetStatusMessage("Serialize failed");
      response.SetCloseConnection(true);
      return ;
    }

    // Protocol MapQueryRankingResponse.
    protocol::MapQueryRankingResponse protocol_ranking;
    protocol_ranking.__set_ranking_(ranking_response.ranking_);
    size_t size = 0;
    const char *ptr = this->packet_.Serialize<protocol::MapQueryRankingResponse>(
        &protocol_ranking, size);
    if(ptr == NULL){
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [MapRequestHandler] Serialize MapQueryRankingResponse failed.",
          __FILE__, __LINE__, __FUNCTION__);
      response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
      response.SetStatusMessage("Serialize failed");
      response.SetCloseConnection(true);
      return ;
    }

    res.__isset.data_ = true;
    res.data_.assign(ptr, size);
  }

  size_t size = 0;
  const char *ptr = this->packet_.Serialize<protocol::MapResponse>(&res, size);
  if(ptr == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Serialize MapResponse failed.",
        __FILE__, __LINE__, __FUNCTION__);
    response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
    response.SetStatusMessage("Serialize failed");
    response.SetCloseConnection(true);
    return ;
  }

  // Protocol response.
  protocol::Response message;
  message.__set_data_type_(protocol::DataType::DATA_TYPE_MAP);
  message.response_.assign(ptr, size);
  ptr = this->packet_.Serialize<protocol::Response>(&message, size);
  if(ptr == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Serialize Response failed.",
        __FILE__, __LINE__, __FUNCTION__);
    response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
    response.SetStatusMessage("Serialize failed");
    response.SetCloseConnection(true);
    return ;
  }

  // Http response.
  response.SetStatusCode(HttpResponse::STATUS_200_OK);
  response.SetStatusMessage("OK");
  response.SetCloseConnection(false);
  response.SetContentType("thrift-0.8");
  response.AddHeader("Server", "Squirrel");
  response.SetBody(ptr, size);
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
      "[MapRequestHandler] Http body size is %d", size);
}

bool MapRequestHandler::OnRequestQueryRange(const protocol::MapRequest &request,
    global::ThriftPacketPtr &packet, const core::TCPConnectionPtr &connection,
    common::StorageType::type storage_type) {
  // Deserialize query request.
  protocol::MapQueryRankingRangeRequest query;
  if(packet->Deserialize<protocol::MapQueryRankingRangeRequest>(&query, request.data_) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Deserialize MapQueryRankingRangeRequest failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Common query reqeust.
  common::MapQueryRankingRangeRequest common_query;
  common_query.__set_start_(query.start_);
  common_query.__set_end_(query.end_);
  common_query.__set_sort_type_((common::MapSortType::type)query.sort_type_);
  size_t size = 0;
  const char *ptr = packet->Serialize<common::MapQueryRankingRangeRequest>(&common_query, size);
  if(ptr == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Serialize MapQueryRankingRangeRequest failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Common request
  common::MapRequest common_request;
  common_request.__set_type_(common::MapRequestType::MAP_REQUEST_TYPE_QUERY_RANKING_RANGE);
  common_request.__isset.data_ = true;
  common_request.data_.assign(ptr, size);
  common_request.__set_key_(request.key_);
  ptr = packet->Serialize<common::MapRequest>(&common_request, size);
  if(ptr == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Serialize MapRequest failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Allocate request id.
  core::uint64 sequence = 0;
  if(dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->AllocateRequest(
        sequence) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] DataProxyClientSingleton allocate request failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Add response list.
  if(this->data_request_handler_->AddWaitResponse(connection, sequence) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] AddWaitResponse failed.",
        __FILE__, __LINE__, __FUNCTION__);
    dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->DeallocateRequest(
        sequence);
    return false;
  }

  // RD type.
  common::RequestRWType::type rd_type = common::RequestRWType::REQUEST_RW_TYPE_READ;

  // Request.
  if(dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->Request(
        common::RequestType::REQUEST_TYPE_MAP, storage_type, rd_type,
        request.key_, std::string(ptr, size), sequence) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] DataProxyClientSingleton Request failed.",
        __FILE__, __LINE__, __FUNCTION__);
    dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->DeallocateRequest(
        sequence);
    core::TCPConnectionPtr connection_tmp;
    this->data_request_handler_->RemoveResponse(sequence, connection_tmp);
    return false;
  }

  return true;
}

void MapRequestHandler::OnResponseQueryRange(const common::MapResponse &map_response,
    HttpResponse &response) {
  // Protocol map response.
  protocol::MapResponse res;
  res.__set_type_((protocol::MapResponseType::type)map_response.result_);

  if(map_response.result_ == common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS) {
    // Deserialize MapQueryRankingRangeResponse.
    common::MapQueryRankingRangeResponse ranking_response;
    if(this->packet_.Deserialize<common::MapQueryRankingRangeResponse>(&ranking_response,
          map_response.data_.data(), map_response.data_.size()) == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [MapRequestHandler] Deserialize MapQueryRankingRangeResponse failed.",
          __FILE__, __LINE__, __FUNCTION__);
      response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
      response.SetStatusMessage("Serialize failed");
      response.SetCloseConnection(true);
      return ;
    }

    typedef std::vector<common::MapQueryRankingRangeInfo> CommonQueryVector;
    // Protocol MapQueryRankingRangeResponse.
    protocol::MapQueryRankingRangeResponse protocol_range;
    protocol::MapQueryRankingRangeInfo element;
    CommonQueryVector::iterator iterator = ranking_response.elements_.begin();
    for(; iterator != ranking_response.elements_.end(); ++iterator) {
      element.__set_ranking_(iterator->ranking_);
      element.__set_ranking_key_(iterator->ranking_key_);
      element.__set_elements_(iterator->elements_);
      protocol_range.elements_.push_back(element);
    }
    size_t size = 0;
    const char *ptr = this->packet_.Serialize<protocol::MapQueryRankingRangeResponse>(
        &protocol_range, size);
    if(ptr == NULL){
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [MapRequestHandler] Serialize MapQueryRankingRangeResponse failed.",
          __FILE__, __LINE__, __FUNCTION__);
      response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
      response.SetStatusMessage("Serialize failed");
      response.SetCloseConnection(true);
      return ;
    }

    res.__isset.data_ = true;
    res.data_.assign(ptr, size);
  }

  size_t size = 0;
  const char *ptr = this->packet_.Serialize<protocol::MapResponse>(&res, size);
  if(ptr == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Serialize MapResponse failed.",
        __FILE__, __LINE__, __FUNCTION__);
    response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
    response.SetStatusMessage("Serialize failed");
    response.SetCloseConnection(true);
    return ;
  }

  // Protocol response.
  protocol::Response message;
  message.__set_data_type_(protocol::DataType::DATA_TYPE_MAP);
  message.response_.assign(ptr, size);
  ptr = this->packet_.Serialize<protocol::Response>(&message, size);
  if(ptr == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Serialize Response failed.",
        __FILE__, __LINE__, __FUNCTION__);
    response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
    response.SetStatusMessage("Serialize failed");
    response.SetCloseConnection(true);
    return ;
  }

  // Http response.
  response.SetStatusCode(HttpResponse::STATUS_200_OK);
  response.SetStatusMessage("OK");
  response.SetCloseConnection(false);
  response.SetContentType("thrift-0.8");
  response.AddHeader("Server", "Squirrel");
  response.SetBody(ptr, size);
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
      "[MapRequestHandler] Http body size is %d", size);
}

bool MapRequestHandler::OnRequestResetRankingKey(const protocol::MapRequest &request,
    global::ThriftPacketPtr &packet, const core::TCPConnectionPtr &connection,
    common::StorageType::type storage_type) {
  // Deserialize reset request.
  protocol::MapResetRankingKeyRequest reset;
  if(packet->Deserialize<protocol::MapResetRankingKeyRequest>(&reset, request.data_) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Deserialize MapQueryMapResetRankingKeyRequest failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Common reset reqeust.
  common::MapResetRankingKeyRequest common_reset;
  common_reset.__set_ranking_key_(reset.ranking_key_);
  common_reset.__set_element_(reset.element_);
  size_t size = 0;
  const char *ptr = packet->Serialize<common::MapResetRankingKeyRequest>(&common_reset, size);
  if(ptr == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Serialize MapResetRankingKeyRequest failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Common request
  common::MapRequest common_request;
  common_request.__set_type_(common::MapRequestType::MAP_REQUEST_TYPE_RESET_RANKING_KEY);
  common_request.__isset.data_ = true;
  common_request.data_.assign(ptr, size);
  common_request.__set_key_(request.key_);
  ptr = packet->Serialize<common::MapRequest>(&common_request, size);
  if(ptr == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Serialize MapRequest failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Allocate request id.
  core::uint64 sequence = 0;
  if(dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->AllocateRequest(
        sequence) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] DataProxyClientSingleton allocate request failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Add response list.
  if(this->data_request_handler_->AddWaitResponse(connection, sequence) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] AddWaitResponse failed.",
        __FILE__, __LINE__, __FUNCTION__);
    dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->DeallocateRequest(
        sequence);
    return false;
  }

  // RD type.
  common::RequestRWType::type rd_type = common::RequestRWType::REQUEST_RW_TYPE_WRITE;

  // Request.
  if(dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->Request(
        common::RequestType::REQUEST_TYPE_MAP, storage_type, rd_type,
        request.key_, std::string(ptr, size), sequence) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] DataProxyClientSingleton Request failed.",
        __FILE__, __LINE__, __FUNCTION__);
    dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->DeallocateRequest(
        sequence);
    core::TCPConnectionPtr connection_tmp;
    this->data_request_handler_->RemoveResponse(sequence, connection_tmp);
    return false;
  }

  return true;
}

void MapRequestHandler::OnResponseResetRankingKey(const common::MapResponse &map_response,
    HttpResponse &response) {
  // Protocol map response.
  protocol::MapResponse res;
  res.__set_type_((protocol::MapResponseType::type)map_response.result_);
  size_t size = 0;
  const char *ptr = this->packet_.Serialize<protocol::MapResponse>(&res, size);
  if(ptr == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Serialize MapResponse failed.",
        __FILE__, __LINE__, __FUNCTION__);
    response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
    response.SetStatusMessage("Serialize failed");
    response.SetCloseConnection(true);
    return ;
  }

  // Protocol response.
  protocol::Response message;
  message.__set_data_type_(protocol::DataType::DATA_TYPE_MAP);
  message.response_.assign(ptr, size);
  ptr = this->packet_.Serialize<protocol::Response>(&message, size);
  if(ptr == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Serialize Response failed.",
        __FILE__, __LINE__, __FUNCTION__);
    response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
    response.SetStatusMessage("Serialize failed");
    response.SetCloseConnection(true);
    return ;
  }

  // Http response.
  response.SetStatusCode(HttpResponse::STATUS_200_OK);
  response.SetStatusMessage("OK");
  response.SetCloseConnection(false);
  response.SetContentType("thrift-0.8");
  response.AddHeader("Server", "Squirrel");
  response.SetBody(ptr, size);
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
      "[MapRequestHandler] Http body size is %d", size);
}

bool MapRequestHandler::OnRequestClear(const protocol::MapRequest &request,
    global::ThriftPacketPtr &packet, const core::TCPConnectionPtr &connection,
    common::StorageType::type storage_type) {
  // Common request
  common::MapRequest common_request;
  common_request.__set_type_(common::MapRequestType::MAP_REQUEST_TYPE_CLEAR);
  common_request.__set_key_(request.key_);
  size_t size = 0;
  const char *ptr = packet->Serialize<common::MapRequest>(&common_request, size);
  if(ptr == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Serialize MapRequest failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Allocate request id.
  core::uint64 sequence = 0;
  if(dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->AllocateRequest(
        sequence) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] DataProxyClientSingleton allocate request failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Add response list.
  if(this->data_request_handler_->AddWaitResponse(connection, sequence) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] AddWaitResponse failed.",
        __FILE__, __LINE__, __FUNCTION__);
    dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->DeallocateRequest(
        sequence);
    return false;
  }

  // RD type.
  common::RequestRWType::type rd_type = common::RequestRWType::REQUEST_RW_TYPE_WRITE;

  // Request.
  if(dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->Request(
        common::RequestType::REQUEST_TYPE_MAP, storage_type, rd_type,
        request.key_, std::string(ptr, size), sequence) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] DataProxyClientSingleton Request failed.",
        __FILE__, __LINE__, __FUNCTION__);
    dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->DeallocateRequest(
        sequence);
    core::TCPConnectionPtr connection_tmp;
    this->data_request_handler_->RemoveResponse(sequence, connection_tmp);
    return false;
  }

  return true;
}

void MapRequestHandler::OnResponseClear(const common::MapResponse &map_response,
    HttpResponse &response) {
  // Protocol map response.
  protocol::MapResponse res;
  res.__set_type_((protocol::MapResponseType::type)map_response.result_);
  size_t size = 0;
  const char *ptr = this->packet_.Serialize<protocol::MapResponse>(&res, size);
  if(ptr == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Serialize MapResponse failed.",
        __FILE__, __LINE__, __FUNCTION__);
    response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
    response.SetStatusMessage("Serialize failed");
    response.SetCloseConnection(true);
    return ;
  }

  // Protocol response.
  protocol::Response message;
  message.__set_data_type_(protocol::DataType::DATA_TYPE_MAP);
  message.response_.assign(ptr, size);
  ptr = this->packet_.Serialize<protocol::Response>(&message, size);
  if(ptr == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Serialize Response failed.",
        __FILE__, __LINE__, __FUNCTION__);
    response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
    response.SetStatusMessage("Serialize failed");
    response.SetCloseConnection(true);
    return ;
  }

  // Http response.
  response.SetStatusCode(HttpResponse::STATUS_200_OK);
  response.SetStatusMessage("OK");
  response.SetCloseConnection(false);
  response.SetContentType("thrift-0.8");
  response.AddHeader("Server", "Squirrel");
  response.SetBody(ptr, size);
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
      "[MapRequestHandler] Http body size is %d", size);
}

bool MapRequestHandler::OnRequestDelete(const protocol::MapRequest &request,
    global::ThriftPacketPtr &packet, const core::TCPConnectionPtr &connection,
    common::StorageType::type storage_type) {
  // Common request
  common::MapRequest common_request;
  common_request.__set_type_(common::MapRequestType::MAP_REQUEST_TYPE_DELETE);
  common_request.__set_key_(request.key_);
  size_t size = 0;
  const char *ptr = packet->Serialize<common::MapRequest>(&common_request, size);
  if(ptr == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Serialize MapRequest failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Allocate request id.
  core::uint64 sequence = 0;
  if(dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->AllocateRequest(
        sequence) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] DataProxyClientSingleton allocate request failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  // Add response list.
  if(this->data_request_handler_->AddWaitResponse(connection, sequence) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] AddWaitResponse failed.",
        __FILE__, __LINE__, __FUNCTION__);
    dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->DeallocateRequest(
        sequence);
    return false;
  }

  // RD type.
  common::RequestRWType::type rd_type = common::RequestRWType::REQUEST_RW_TYPE_WRITE;

  // Request.
  if(dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->Request(
        common::RequestType::REQUEST_TYPE_MAP, storage_type, rd_type,
        request.key_, std::string(ptr, size), sequence) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] DataProxyClientSingleton Request failed.",
        __FILE__, __LINE__, __FUNCTION__);
    dataproxy::client::DataProxyClientSingleton::GetDataProxyClient()->DeallocateRequest(
        sequence);
    core::TCPConnectionPtr connection_tmp;
    this->data_request_handler_->RemoveResponse(sequence, connection_tmp);
    return false;
  }

  return true;
}

void MapRequestHandler::OnResponseDelete(const common::MapResponse &map_response,
    HttpResponse &response) {
  // Protocol map response.
  protocol::MapResponse res;
  res.__set_type_((protocol::MapResponseType::type)map_response.result_);
  size_t size = 0;
  const char *ptr = this->packet_.Serialize<protocol::MapResponse>(&res, size);
  if(ptr == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Serialize MapResponse failed.",
        __FILE__, __LINE__, __FUNCTION__);
    response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
    response.SetStatusMessage("Serialize failed");
    response.SetCloseConnection(true);
    return ;
  }

  // Protocol response.
  protocol::Response message;
  message.__set_data_type_(protocol::DataType::DATA_TYPE_MAP);
  message.response_.assign(ptr, size);
  ptr = this->packet_.Serialize<protocol::Response>(&message, size);
  if(ptr == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [MapRequestHandler] Serialize Response failed.",
        __FILE__, __LINE__, __FUNCTION__);
    response.SetStatusCode(HttpResponse::STATUS_400_BAD_REQUEST);
    response.SetStatusMessage("Serialize failed");
    response.SetCloseConnection(true);
    return ;
  }

  // Http response.
  response.SetStatusCode(HttpResponse::STATUS_200_OK);
  response.SetStatusMessage("OK");
  response.SetCloseConnection(false);
  response.SetContentType("thrift-0.8");
  response.AddHeader("Server", "Squirrel");
  response.SetBody(ptr, size);
  AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_DEBUG,
      "[MapRequestHandler] Http body size is %d", size);
}

}  // namespace gateway

}  // namespace squirrel

