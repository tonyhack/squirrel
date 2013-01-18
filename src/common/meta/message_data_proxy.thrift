namespace cpp squirrel.common

include "server_type.thrift"
include "request_type.thrift"

enum MessageDataProxyType {
  MESSAGE_DATA_PROXY_TYPE_LOGIN_REQ,
  MESSAGE_DATA_PROXY_TYPE_LOGIN_RES,
  MESSAGE_DATA_PROXY_TYPE_LOGOUT_REQ,
  MESSAGE_DATA_PROXY_TYPE_LOGOUT_RES,
  MESSAGE_DATA_PROXY_TYPE_DATA_REQUEST,
  MESSAGE_DATA_PROXY_TYPE_DATA_RESPONSE,

  MESSAGE_DATA_PROXY_TYPE_MAX,
}

struct MessageDataProxy {
  1 : required MessageDataProxyType type_,
  2 : optional binary message_;
}

// type: MESSAGE_DATA_PROXY_TYPE_LOGIN_REQ
// meaning: 请求登录dataproxy服务器
// direction: other->dataproxy
struct MessageDataProxyLoginReq {
  1 : required server_type.ServerType type_,
  2 : required i32 id_,
  3 : required string host_,
  4 : required i16 port_,
}


// type: MESSAGE_DATA_PROXY_TYPE_LOGIN_RES
// meaning: 返回登录dataproxy服务器结果
// direction: dataproxy->other
struct MessageDataProxyLoginRes {
  1 : required bool result_,
}


// type: MESSAGE_DATA_PROXY_TYPE_LOGOUT_REQ
// meaning: 请求退出dataproxy服务器
// direction: other->dataproxy


// type: MESSAGE_DATA_PROXY_TYPE_LOGOUT_RES
// meaning: 返回退出dataproxy服务器结果
// direction: dataproxy->other
struct MessageDataProxyLogoutRes {
  1 : required bool result_,
}



// Read/Write type, use to divide into different thread.
enum RequestRWType {
  REQUEST_RW_TYPE_READ,
  REQUEST_RW_TYPE_WRITE,

  REQUEST_RW_TYPE_MAX,
}

// type: MESSAGE_DATA_PROXY_TYPE_DATA_REQUEST
// meaning: 向dataproxy服务器做数据请求
// direction: other->dataproxy
struct MessageDataProxyDataRequest {
  1 : required i64 request_id_,
  2 : required request_type.RequestType type_,
  3 : required request_type.StorageType storage_type_,
  4 : required RequestRWType rd_type_,
  5 : required binary key_,
  6 : required binary request_,
}

// type: MESSAGE_DATA_PROXY_TYPE_DATA_RESPONSE
// meaning: dataproxy服务器返回数据请求结果
// direction: dataproxy->other
struct MessageDataProxyDataResponse {
  1 : required i64 request_id_,
  2 : required request_type.RequestType type_,
  3 : required binary key_,
  4 : required binary response_,
}

