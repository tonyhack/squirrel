namespace as3 squirrel.protocol
namespace php squirrel.protocol
namespace cpp squirrel.protocol

enum DataType {
  DATA_TYPE_KEYVALUE,                       // key-value type.
  DATA_TYPE_NUMBER,                         // number type.
  DATA_TYPE_LIST,                           // list type.
  DATA_TYPE_MAP,                            // map type.
}

enum StorageType {
  STORAGE_TYPE_CACHE = 0x1,                 // only cache.
  STORAGE_TYPE_DISK = 0x2,                  // only disk.
}

struct Request {
  1 : required DataType data_type_,         // enum DataType.
  2 : required StorageType storage_type_,   // enum StorageType.
  3 : required binary request_,             // according data_type_.
}

struct Response {
  1 : required DataType data_type_,         // enum DataType.
  2 : required binary response_,            // according data_type_.
}

