namespace as3 squirrel.protocol
namespace php squirrel.protocol
namespace cpp squirrel.protocol

enum ListRequestType {
  LIST_REQUEST_TYPE_PUSH,                       // push a element.
  LIST_REQUEST_TYPE_POP,                        // pop a element.
  LIST_REQUEST_TYPE_REMOVE,                     // remove a element.
  LIST_REQUEST_TYPE_CLEAR,                      // clear all elements.
  LIST_REQUEST_TYPE_DELETE,                     // delete the list.

  LIST_REQUEST_TYPE_MAX,
}

enum ListResponseType {
  LIST_RESPONSE_TYPE_SUCCESS,
  LIST_RESPONSE_TYPE_KEY_NOT_EXIST,
  LIST_RESPONSE_TYPE_ELEM_ALREDY_EXIST,
  LIST_RESPONSE_TYPE_ELEM_NOT_EXIST,
  LIST_RESPONSE_TYPE_EMPTY,
  LIST_RESPONSE_TYPE_DISCONNECTED,
  LIST_RESPONSE_TYPE_UNKNOWN,

  LIST_RESPONSE_TYPE_MAX,
}

struct ListRequest {
  1 : required ListRequestType type_,           // enum ListRequestType.
  2 : required binary key_,                     // key.
  3 : optional binary data_,                    // element(when LIST_REQUEST_TYPE_PUSH, LIST_REQUEST_TYPE_REMOVE).
}

struct ListResponse {
  1 : required ListResponseType result_,        // enum ListResponseType.
  2 : optional binary data_,                    // element(when LIST_REQUEST_TYPE_POP).
}

