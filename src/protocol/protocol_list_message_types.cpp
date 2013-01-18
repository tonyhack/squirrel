/**
 * Autogenerated by Thrift Compiler (0.8.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#include "protocol_list_message_types.h"

namespace squirrel { namespace protocol {

int _kListRequestTypeValues[] = {
  ListRequestType::LIST_REQUEST_TYPE_PUSH,
  ListRequestType::LIST_REQUEST_TYPE_POP,
  ListRequestType::LIST_REQUEST_TYPE_REMOVE,
  ListRequestType::LIST_REQUEST_TYPE_CLEAR,
  ListRequestType::LIST_REQUEST_TYPE_DELETE,
  ListRequestType::LIST_REQUEST_TYPE_MAX
};
const char* _kListRequestTypeNames[] = {
  "LIST_REQUEST_TYPE_PUSH",
  "LIST_REQUEST_TYPE_POP",
  "LIST_REQUEST_TYPE_REMOVE",
  "LIST_REQUEST_TYPE_CLEAR",
  "LIST_REQUEST_TYPE_DELETE",
  "LIST_REQUEST_TYPE_MAX"
};
const std::map<int, const char*> _ListRequestType_VALUES_TO_NAMES(::apache::thrift::TEnumIterator(6, _kListRequestTypeValues, _kListRequestTypeNames), ::apache::thrift::TEnumIterator(-1, NULL, NULL));

int _kListResponseTypeValues[] = {
  ListResponseType::LIST_RESPONSE_TYPE_SUCCESS,
  ListResponseType::LIST_RESPONSE_TYPE_KEY_NOT_EXIST,
  ListResponseType::LIST_RESPONSE_TYPE_ELEM_ALREDY_EXIST,
  ListResponseType::LIST_RESPONSE_TYPE_ELEM_NOT_EXIST,
  ListResponseType::LIST_RESPONSE_TYPE_EMPTY,
  ListResponseType::LIST_RESPONSE_TYPE_DISCONNECTED,
  ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN,
  ListResponseType::LIST_RESPONSE_TYPE_MAX
};
const char* _kListResponseTypeNames[] = {
  "LIST_RESPONSE_TYPE_SUCCESS",
  "LIST_RESPONSE_TYPE_KEY_NOT_EXIST",
  "LIST_RESPONSE_TYPE_ELEM_ALREDY_EXIST",
  "LIST_RESPONSE_TYPE_ELEM_NOT_EXIST",
  "LIST_RESPONSE_TYPE_EMPTY",
  "LIST_RESPONSE_TYPE_DISCONNECTED",
  "LIST_RESPONSE_TYPE_UNKNOWN",
  "LIST_RESPONSE_TYPE_MAX"
};
const std::map<int, const char*> _ListResponseType_VALUES_TO_NAMES(::apache::thrift::TEnumIterator(8, _kListResponseTypeValues, _kListResponseTypeNames), ::apache::thrift::TEnumIterator(-1, NULL, NULL));

const char* ListRequest::ascii_fingerprint = "8A168E64138582CD39890B77E0F15FF4";
const uint8_t ListRequest::binary_fingerprint[16] = {0x8A,0x16,0x8E,0x64,0x13,0x85,0x82,0xCD,0x39,0x89,0x0B,0x77,0xE0,0xF1,0x5F,0xF4};

uint32_t ListRequest::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;

  bool isset_type_ = false;
  bool isset_key_ = false;

  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          int32_t ecast0;
          xfer += iprot->readI32(ecast0);
          this->type_ = (ListRequestType::type)ecast0;
          isset_type_ = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readBinary(this->key_);
          isset_key_ = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 3:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readBinary(this->data_);
          this->__isset.data_ = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  if (!isset_type_)
    throw TProtocolException(TProtocolException::INVALID_DATA);
  if (!isset_key_)
    throw TProtocolException(TProtocolException::INVALID_DATA);
  return xfer;
}

uint32_t ListRequest::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("ListRequest");
  xfer += oprot->writeFieldBegin("type_", ::apache::thrift::protocol::T_I32, 1);
  xfer += oprot->writeI32((int32_t)this->type_);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("key_", ::apache::thrift::protocol::T_STRING, 2);
  xfer += oprot->writeBinary(this->key_);
  xfer += oprot->writeFieldEnd();
  if (this->__isset.data_) {
    xfer += oprot->writeFieldBegin("data_", ::apache::thrift::protocol::T_STRING, 3);
    xfer += oprot->writeBinary(this->data_);
    xfer += oprot->writeFieldEnd();
  }
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

const char* ListResponse::ascii_fingerprint = "24652790C81ECE22B629CB60A19F1E93";
const uint8_t ListResponse::binary_fingerprint[16] = {0x24,0x65,0x27,0x90,0xC8,0x1E,0xCE,0x22,0xB6,0x29,0xCB,0x60,0xA1,0x9F,0x1E,0x93};

uint32_t ListResponse::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;

  bool isset_result_ = false;

  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          int32_t ecast1;
          xfer += iprot->readI32(ecast1);
          this->result_ = (ListResponseType::type)ecast1;
          isset_result_ = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readBinary(this->data_);
          this->__isset.data_ = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  if (!isset_result_)
    throw TProtocolException(TProtocolException::INVALID_DATA);
  return xfer;
}

uint32_t ListResponse::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("ListResponse");
  xfer += oprot->writeFieldBegin("result_", ::apache::thrift::protocol::T_I32, 1);
  xfer += oprot->writeI32((int32_t)this->result_);
  xfer += oprot->writeFieldEnd();
  if (this->__isset.data_) {
    xfer += oprot->writeFieldBegin("data_", ::apache::thrift::protocol::T_STRING, 2);
    xfer += oprot->writeBinary(this->data_);
    xfer += oprot->writeFieldEnd();
  }
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

}} // namespace