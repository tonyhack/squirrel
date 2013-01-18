//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-11 17:43:09.
// File name: data_request_handler.h
//
// Description: 
// Define class DataRequestHandler.
//

#ifndef _squirrel_gateway__DATA__REQUEST__HANDLER__H
#define _squirrel_gateway__DATA__REQUEST__HANDLER__H

#include "common/message_data_proxy_types.h"
#include "core/tcp_connection.h"
#include "core/base/byte_buffer_dynamic.h"
#include "core/base/mutex.h"
#include "core/base/noncopyable.h"
#include "core/base/types.h"
#include "gateway_server/map_request_handler.h"
#include "global/thrift_packet.h"
#include "protocol/message_types.h"

namespace squirrel {

namespace gateway {

class DataRequestHandler : public core::Noncopyable {
  friend class MapRequestHandler;
  typedef std::map<core::uint64, core::TCPConnectionPtr> WaitResponseMap;
 public:
  DataRequestHandler();
  ~DataRequestHandler();

  bool OnRequest(const protocol::Request &request, global::ThriftPacketPtr &packet,
      const core::TCPConnectionPtr &connection);
  void OnResponse(const common::MessageDataProxyDataResponse *response);

 private:
  bool AddWaitResponse(const core::TCPConnectionPtr &connection, core::uint64 sequence);
  bool RemoveResponse(core::uint64 sequence, core::TCPConnectionPtr &connection);

  bool OnKeyvalueRequest(const protocol::Request &request,
      global::ThriftPacketPtr &packet, const core::TCPConnectionPtr &connection);
  void OnKeyvalueResponse(const core::TCPConnectionPtr &connection,
      const std::string &response);

  bool OnNumberRequest(const protocol::Request &request,
      global::ThriftPacketPtr &packet, const core::TCPConnectionPtr &connection);
  void OnNumberResponse(const core::TCPConnectionPtr &connection,
      const std::string &response);

  bool OnListRequest(const protocol::Request &request,
      global::ThriftPacketPtr &packet, const core::TCPConnectionPtr &connection);
  void OnListResponse(const core::TCPConnectionPtr &connection,
      const std::string &response);

  bool OnMapRequest(const protocol::Request &request,
      global::ThriftPacketPtr &packet, const core::TCPConnectionPtr &connection);
  void OnMapResponse(const core::TCPConnectionPtr &connection,
      const std::string &response);

  void OnDefaultResponse(const core::TCPConnectionPtr &connection,
      const std::string &response);

  WaitResponseMap wait_responses_;
  core::Mutex mutex_;

  global::ThriftPacket packet_;
  core::Mutex packet_mutex_;

  core::ByteBufferDynamic buffer_;

  MapRequestHandler map_request_handler_;
};

}  // namespace gateway

}  // namespace squirrel

#endif  // _squirrel_gateway__DATA__REQUEST__HANDLER__H

