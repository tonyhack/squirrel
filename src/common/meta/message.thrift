namespace cpp squirrel.common

enum MessageType {
  MESSAGE_TYPE_SCHEDULE,
  MESSAGE_TYPE_ROUTE,
  MESSAGE_TYPE_DATA_PROXY,
  MESSAGE_TYPE_GATEWAY,
}

struct Message {
  1 : required MessageType type_,
  2 : required binary message_,
}

