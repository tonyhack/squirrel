namespace cpp squirrel.common

enum NumberRequestType {
  NUMBER_REQUEST_TYPE_INC,
  NUMBER_REQUEST_TYPE_DEC,
  NUMBER_REQUEST_TYPE_RESET,
  NUMBER_REQUEST_TYPE_DELETE,

  NUMBER_REQUEST_TYPE_MAX,
}

enum NumberResponseType {
  NUMBER_RESPONSE_TYPE_SUCCESS,
  NUMBER_RESPONSE_TYPE_KEY_NOT_EXIST,
  NUMBER_RESPONSE_TYPE_DISCONNECTED,
  NUMBER_RESPONSE_TYPE_UNKNOWN,

  NUMBER_RESPONSE_TYPE_MAX,
}

struct NumberRequest {
  1 : required NumberRequestType type_,           // enum NumberRequestType.
  2 : optional string increment_,                 // increase value.
}

struct NumberResponse {
  1 : required NumberRequestType type_,
  2 : required NumberResponseType result_,        // enum NumberResponseType.
  3 : optional string value_,                     // value.
}
