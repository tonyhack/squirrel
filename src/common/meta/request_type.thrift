namespace cpp squirrel.common

enum RequestType {
  REQUEST_TYPE_KEYVALUE,
  REQUEST_TYPE_NUMBER,
  REQUEST_TYPE_LIST,
  REQUEST_TYPE_MAP,

  REQUEST_TYPE_MAX,
}

enum StorageType {
  STORAGE_TYPE_CACHE = 0x1,
  STORAGE_TYPE_DISK = 0x2,
}

