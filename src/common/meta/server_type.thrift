namespace cpp squirrel.common

enum ServerType {
  SERVER_TYPE_SCHEDULE,
  SERVER_TYPE_GATEWAY,
  SERVER_TYPE_ROUTE,
  SERVER_TYPE_DATA_PROXY,
  SERVER_TYPE_MAX,
}

struct ServerSignature {
  1 : required ServerType type_,
  2 : required i32 id_,
  3 : required string host_,
  4 : required i16 port_,
}

