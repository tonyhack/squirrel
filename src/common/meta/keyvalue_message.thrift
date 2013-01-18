namespace cpp squirrel.common

enum KeyvalueRequestType {
  KEYVALUE_REQUEST_TYPE_SET,
  KEYVALUE_REQUEST_TYPE_GET,
  KEYVALUE_REQUEST_TYPE_DELETE,
  KEYVALUE_REQUEST_TYPE_SYNC_REDIS,

  KEYVALUE_REQUEST_TYPE_MAX,
}

enum KeyvalueResponseType {
  KEYVALUE_RESPONSE_TYPE_SUCCESS,
  KEYVALUE_RESPONSE_TYPE_KEY_NOT_EXIST,
  KEYVALUE_RESPONSE_TYPE_DISCONNECTED,
  KEYVALUE_RESPONSE_TYPE_UNKNOWN,

  KEYVALUE_RESPONSE_TYPE_MAX,
}

struct KeyvalueRequest {
  1 : required KeyvalueRequestType type_,
  2 : optional binary value_,
}

struct KeyvalueResponse {
  1 : required KeyvalueRequestType type_,
  2 : required KeyvalueResponseType result_,
  3 : optional binary value_,
}
