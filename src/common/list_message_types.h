/**
 * Autogenerated by Thrift Compiler (0.8.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef list_message_TYPES_H
#define list_message_TYPES_H

#include <Thrift.h>
#include <TApplicationException.h>
#include <protocol/TProtocol.h>
#include <transport/TTransport.h>



namespace squirrel { namespace common {

struct ListRequestType {
  enum type {
    LIST_REQUEST_TYPE_PUSH = 0,
    LIST_REQUEST_TYPE_POP = 1,
    LIST_REQUEST_TYPE_REMOVE = 2,
    LIST_REQUEST_TYPE_CLEAR = 3,
    LIST_REQUEST_TYPE_DELETE = 4,
    LIST_REQUEST_TYPE_MAX = 5
  };
};

extern const std::map<int, const char*> _ListRequestType_VALUES_TO_NAMES;

struct ListResponseType {
  enum type {
    LIST_RESPONSE_TYPE_SUCCESS = 0,
    LIST_RESPONSE_TYPE_KEY_NOT_EXIST = 1,
    LIST_RESPONSE_TYPE_ELEM_ALREDY_EXIST = 2,
    LIST_RESPONSE_TYPE_ELEM_NOT_EXIST = 3,
    LIST_RESPONSE_TYPE_EMPTY = 4,
    LIST_RESPONSE_TYPE_DISCONNECTED = 5,
    LIST_RESPONSE_TYPE_UNKNOWN = 6,
    LIST_RESPONSE_TYPE_MAX = 7
  };
};

extern const std::map<int, const char*> _ListResponseType_VALUES_TO_NAMES;

typedef struct _ListRequest__isset {
  _ListRequest__isset() : data_(false) {}
  bool data_;
} _ListRequest__isset;

class ListRequest {
 public:

  static const char* ascii_fingerprint; // = "24652790C81ECE22B629CB60A19F1E93";
  static const uint8_t binary_fingerprint[16]; // = {0x24,0x65,0x27,0x90,0xC8,0x1E,0xCE,0x22,0xB6,0x29,0xCB,0x60,0xA1,0x9F,0x1E,0x93};

  ListRequest() : type_((ListRequestType::type)0), data_("") {
  }

  virtual ~ListRequest() throw() {}

  ListRequestType::type type_;
  std::string data_;

  _ListRequest__isset __isset;

  void __set_type_(const ListRequestType::type val) {
    type_ = val;
  }

  void __set_data_(const std::string& val) {
    data_ = val;
    __isset.data_ = true;
  }

  bool operator == (const ListRequest & rhs) const
  {
    if (!(type_ == rhs.type_))
      return false;
    if (__isset.data_ != rhs.__isset.data_)
      return false;
    else if (__isset.data_ && !(data_ == rhs.data_))
      return false;
    return true;
  }
  bool operator != (const ListRequest &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const ListRequest & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _ListResponse__isset {
  _ListResponse__isset() : data_(false) {}
  bool data_;
} _ListResponse__isset;

class ListResponse {
 public:

  static const char* ascii_fingerprint; // = "19AAB18E981C4EB11AEBB34F40FF0939";
  static const uint8_t binary_fingerprint[16]; // = {0x19,0xAA,0xB1,0x8E,0x98,0x1C,0x4E,0xB1,0x1A,0xEB,0xB3,0x4F,0x40,0xFF,0x09,0x39};

  ListResponse() : request_type_((ListRequestType::type)0), result_((ListResponseType::type)0), data_("") {
  }

  virtual ~ListResponse() throw() {}

  ListRequestType::type request_type_;
  ListResponseType::type result_;
  std::string data_;

  _ListResponse__isset __isset;

  void __set_request_type_(const ListRequestType::type val) {
    request_type_ = val;
  }

  void __set_result_(const ListResponseType::type val) {
    result_ = val;
  }

  void __set_data_(const std::string& val) {
    data_ = val;
    __isset.data_ = true;
  }

  bool operator == (const ListResponse & rhs) const
  {
    if (!(request_type_ == rhs.request_type_))
      return false;
    if (!(result_ == rhs.result_))
      return false;
    if (__isset.data_ != rhs.__isset.data_)
      return false;
    else if (__isset.data_ && !(data_ == rhs.data_))
      return false;
    return true;
  }
  bool operator != (const ListResponse &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const ListResponse & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

}} // namespace

#endif