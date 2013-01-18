namespace as3 squirrel.protocol
namespace php squirrel.protocol
namespace cpp squirrel.protocol

enum KeyvalueRequestType {
  KEYVALUE_REQUEST_TYPE_SET,                        // set.
  KEYVALUE_REQUEST_TYPE_GET,                        // get.
  KEYVALUE_REQUEST_TYPE_DELETE,                     // delete.

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
  1 : required KeyvalueRequestType type_,           // enum KeyvalueRequestType.
  2 : required list<binary> key_list_,              // list of key.
  3 : optional list<binary> value_list_,            // list of value(when type_ is KEYVALUE_REQUEST_TYPE_SET),
}

struct KeyvalueResponse {
  1 : required KeyvalueResponseType result_,        // enum KeyvalueResponseType.
  2 : optional list<binary> value_list_,            // list of value(when KEYVALUE_REQUEST_TYPE_GET).
}

