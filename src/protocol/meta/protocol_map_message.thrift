namespace as3 squirrel.protocol
namespace php squirrel.protocol
namespace cpp squirrel.protocol

enum MapRequestType {
  MAP_REQUEST_TYPE_INSERT,                      // insert a element.
  MAP_REQUEST_TYPE_REMOVE,                      // remove a element.
  MAP_REQUEST_TYPE_QUERY_RANKING,               // query a element's ranking value.
  MAP_REQUEST_TYPE_QUERY_RANKING_RANGE,         // query all elements in the range of ranking values.
  MAP_REQUEST_TYPE_RESET_RANKING_KEY,           // reset the element's ranking key.
  MAP_REQUEST_TYPE_CLEAR,                       // clear all the element.
  MAP_REQUEST_TYPE_DELETE,                      // delete the list.

  MAP_REQUEST_TYPE_MAX,
}

enum MapResponseType {
  MAP_RESPONSE_TYPE_SUCCESS,
  MAP_RESPONSE_TYPE_KEY_NOT_EXIST,
  MAP_RESPONSE_TYPE_ELEM_ALREDY_EXIST,
  MAP_RESPONSE_TYPE_ELEM_NOT_EXIST,
  MAP_RESPONSE_TYPE_DISCONNECTED,
  MAP_RESPONSE_TYPE_UNKNOWN,

  MAP_RESPONSE_TYPE_MAX,
}

enum MapSortType {
  MAP_SORT_TYPE_ASC,                            // asc.
  MAP_SORT_TYPE_DESC,                           // desc.

  MAP_SORT_TYPE_MAX,
}



struct MapRequest {
  1 : required MapRequestType type_,            // enum MapRequestType.
  2 : required binary key_,
  3 : optional binary data_,                    // 
}

struct MapResponse {
  1 : required MapResponseType type_,           // enum MapResponseType.
  2 : optional binary data_,                    // 
}



// MAP_REQUEST_TYPE_INSERT.
struct MapInsertRequest {
  1 : required string ranking_key_,             // ranking key.
  2 : required binary element_,                 // element.
}



// MAP_REQUEST_TYPE_RESET_RANKING_KEY
struct MapResetRankingKeyRequest {
  1 : required string ranking_key_,             // ranking key.
  2 : required binary element_,                 // element.
}



// MAP_REQUEST_TYPE_REMOVE
struct MapRemoveRequest {
  1 : required binary element_,                 // element.
}



// MAP_REQUEST_TYPE_QUERY_RANKING
struct MapQueryRankingRequest {
  1 : required binary element_,                 // element.
  2 : required MapSortType sort_type_,          // enum MapSortType.
}

struct MapQueryRankingResponse {
  1 : required string ranking_,                 // ranking.
}



// MAP_REQUEST_TYPE_QUERY_RANKING_RANGE
struct MapQueryRankingRangeRequest {
  1 : required string start_,                   // start of range.
  2 : required string end_,                     // end of range.
  3 : required MapSortType sort_type_,          // enum MapSortType.
}

struct MapQueryRankingRangeInfo {
  1 : required string ranking_,                 // ranking.
  2 : required string ranking_key_,             // ranking key.
  3 : required binary elements_,                // element.
}

struct MapQueryRankingRangeResponse {
  1 : required list<MapQueryRankingRangeInfo> elements_,
}

