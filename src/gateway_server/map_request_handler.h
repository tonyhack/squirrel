//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-10-01 01:17:59.
// File name: map_request_handler.h
//
// Description: 
// Define class MapRequestHandler.
//

#ifndef _squirrel_gateway__MAP__REQUEST__HANDLER__H
#define _squirrel_gateway__MAP__REQUEST__HANDLER__H

#include "common/message_data_proxy_types.h"
#include "common/map_message_types.h"
#include "core/tcp_connection.h"
#include "core/base/byte_buffer_dynamic.h"
#include "core/base/mutex.h"
#include "core/base/noncopyable.h"
#include "core/base/types.h"
#include "gateway_server/http_response.h"
#include "global/thrift_packet.h"
#include "protocol/message_types.h"
#include "protocol/protocol_map_message_types.h"

namespace squirrel {

namespace gateway {

class DataRequestHandler;

class MapRequestHandler : public core::Noncopyable {
 public:
  MapRequestHandler();
  ~MapRequestHandler();

  void Initialize(DataRequestHandler *data_request_handler) {
    this->data_request_handler_ = data_request_handler;
  }

  bool OnMapRequest(const protocol::Request &request,
      global::ThriftPacketPtr &packet, const core::TCPConnectionPtr &connection);
  void OnMapResponse(const core::TCPConnectionPtr &connection,
      const std::string &response);

 private:
  bool OnRequestInsert(const protocol::MapRequest &request,
      global::ThriftPacketPtr &packet, const core::TCPConnectionPtr &connection,
      common::StorageType::type storage_type);
  void OnResponseInsert(const common::MapResponse &map_response,
      HttpResponse &response);

  bool OnRequestRemove(const protocol::MapRequest &request,
      global::ThriftPacketPtr &packet, const core::TCPConnectionPtr &connection,
      common::StorageType::type storage_type);
  void OnResponseRemove(const common::MapResponse &map_response,
      HttpResponse &response);

  bool OnRequestQueryRank(const protocol::MapRequest &request,
      global::ThriftPacketPtr &packet, const core::TCPConnectionPtr &connection,
      common::StorageType::type storage_type);
  void OnResponseQueryRank(const common::MapResponse &map_response,
      HttpResponse &response);

  bool OnRequestQueryRange(const protocol::MapRequest &request,
      global::ThriftPacketPtr &packet, const core::TCPConnectionPtr &connection,
      common::StorageType::type storage_type);
  void OnResponseQueryRange(const common::MapResponse &map_response,
      HttpResponse &response);

  bool OnRequestResetRankingKey(const protocol::MapRequest &request,
      global::ThriftPacketPtr &packet, const core::TCPConnectionPtr &connection,
      common::StorageType::type storage_type);
  void OnResponseResetRankingKey(const common::MapResponse &map_response,
      HttpResponse &response);

  bool OnRequestClear(const protocol::MapRequest &request,
      global::ThriftPacketPtr &packet, const core::TCPConnectionPtr &connection,
      common::StorageType::type storage_type);
  void OnResponseClear(const common::MapResponse &map_response,
      HttpResponse &response);

  bool OnRequestDelete(const protocol::MapRequest &request,
      global::ThriftPacketPtr &packet, const core::TCPConnectionPtr &connection,
      common::StorageType::type storage_type);
  void OnResponseDelete(const common::MapResponse &map_response,
      HttpResponse &response);

  DataRequestHandler *data_request_handler_;

  global::ThriftPacket packet_;
  core::Mutex packet_mutex_;

  core::ByteBufferDynamic buffer_;
};

}  // namespace gateway

}  // namespace squirrel

#endif  // _squirrel_gateway__MAP__REQUEST__HANDLER__H

