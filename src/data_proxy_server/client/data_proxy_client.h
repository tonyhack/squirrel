//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-10 14:25:20.
// File name: data_proxy_client.h
//
// Description: 
// Define class DataProxyClient.
//

#ifndef _squirrel_dataproxy_client__DATA__PROXY__CLIENT__H
#define _squirrel_dataproxy_client__DATA__PROXY__CLIENT__H

#include <map>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include "common/message_data_proxy_types.h"
#include "core/callbacks.h"
#include "core/event_loop.h"
#include "core/inet_address.h"
#include "core/tcp_connection.h"
#include "core/timer_handler.h"
#include "core/base/byte_buffer_dynamic.h"
#include "core/base/noncopyable.h"
#include "data_proxy_server/client/data_proxy_client_session.h"
#include "data_proxy_server/client/default_configure.h"
#include "global/async_logging_singleton.h"
#include "global/sequence_allocator.h"
#include "global/thrift_packet.h"

namespace squirrel {

using namespace global;

namespace dataproxy {

namespace client {

class DataProxyClient : public core::Noncopyable {
  typedef std::map<core::uint32, DataProxyClientSessionPtr> SessionMap;
 public:
  typedef boost::function<void(const common::MessageDataProxyDataResponse *)>
    ResponseCallback;

  DataProxyClient();
  ~DataProxyClient();

  bool Initialize(core::EventLoop *loop, core::uint32 server_id,
      common::ServerType::type server_type, const core::InetAddress &local_address);

  void Start();
  void Stop();

  inline common::ServerType::type GetHostType() const { return this->host_type_; }
  inline core::uint32 GetHostID() const { return this->host_id_; }
  inline const std::string &GetHostIP() const { return this->host_ip_; }
  inline core::uint16 GetHostPort() const { return this->host_port_; }

  inline void SetResponseCallback(const ResponseCallback &callback) {
    this->response_callback_ = callback;
  }

  bool AddSession(const DataProxyClientSessionPtr &session);
  bool RemoveSession(core::uint32 server_id);

  bool Request(common::RequestType::type request_type,
      common::StorageType::type storage_type, common::RequestRWType::type rd_type,
      const std::string &key, const std::string &request, core::uint64 request_id);
  void OnResponse(const common::MessageDataProxyDataResponse *response);

  bool AllocateRequest(core::uint64 &request) {
    return this->sequence_allcator_.Allocate(request);
  }
  void DeallocateRequest(core::uint64 request) {
    this->sequence_allcator_.Deallocate(request);
  }

 private:
  core::EventLoop *loop_;

  common::ServerType::type host_type_;
  core::uint32 host_id_;
  std::string host_ip_;
  core::uint16 host_port_;

  SessionMap sessions_;

  global::MTSequenceAllocator sequence_allcator_;
  global::ThriftPacket packet_;
  core::Mutex mutex_;

  ResponseCallback response_callback_;
};

class DataProxyClientSingleton : public core::Noncopyable {
 public:
  DataProxyClientSingleton() {}
  ~DataProxyClientSingleton() {}

  static inline DataProxyClient *GetDataProxyClient() {
    return client_;
  }
  static inline void SetDataProxyClient(DataProxyClient *client) {
    client_ = client;
  }

 private:
  static DataProxyClient *client_;
};

}  // namespace client

}  // namespace dataproxy

}  // namespace squirrel

#endif  // _squirrel_dataproxy_client__DATA__PROXY__CLIENT__H

