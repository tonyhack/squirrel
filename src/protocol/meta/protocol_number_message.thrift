namespace as3 squirrel.protocol
namespace php squirrel.protocol
namespace cpp squirrel.protocol

enum NumberRequestType {
  NUMBER_REQUEST_TYPE_INC,                        // increase.
  NUMBER_REQUEST_TYPE_DEC,                        // decrease.
  NUMBER_REQUEST_TYPE_RESET,                      // reset value to 0.
  NUMBER_REQUEST_TYPE_DELETE,                     // delete.

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
  2 : required binary key_,                       // key.
  3 : optional string increment_,                 // increase value.
}

struct NumberResponse {
  1 : required NumberResponseType result_,        // enum NumberResponseType.
  2 : optional string value_,                     // value.
}

