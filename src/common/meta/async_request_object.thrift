namespace cpp squirrel.common

include "request_type.thrift"   

struct AsyncRequestObject {
  1 : required i64 request_id_,
  2 : required i64 terminal_guid_,
  3 : required request_type.RequestType type_,
  4 : required request_type.StorageType storage_type_,
  5 : required binary key_,
  6 : required i32 key_hash_value_,
  7 : required binary request_,
}

