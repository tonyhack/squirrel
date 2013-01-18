//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-06 15:02:48.
// File name: procedure_type.h
//
// Description: 
// Define procedure type.
//

#ifndef _squirrel_dataproxy_storage__PROCEDURE__TYPE__H
#define _squirrel_dataproxy_storage__PROCEDURE__TYPE__H

namespace squirrel {

namespace dataproxy {

namespace storage {

struct ProcedureType {
  enum type {
    PROCEDURE_KEYVEALUE_SET = 0,
    PROCEDURE_KEYVEALUE_GET,
    PROCEDURE_KEYVEALUE_DELETE,

    PROCEDURE_NUMBER_INCREASE,
    PROCEDURE_NUMBER_DECREASE,
    PROCEDURE_NUMBER_RESET,
    PROCEDURE_NUMBER_DELETE,

    PROCEDURE_LIST_PUSH,
    PROCEDURE_LIST_POP,
    PROCEDURE_LIST_REMOVE,
    PROCEDURE_LIST_CLEAR,
    PROCEDURE_LIST_DELETE,

    PROCEDURE_MAP_INSERT,
    PROCEDURE_MAP_REMOVE,
    PROCEDURE_MAP_QUERY_RANK,
    PROCEDURE_MAP_QUERY_RANGE,
    PROCEDURE_MAP_CLEAR,
    PROCEDURE_MAP_DELETE,

    PROCEDURE_MAX,
  };
};

}  // namespace storage

}  // namespace dataproxy

}  // namespace squirrel

#endif  // _squirrel_dataproxy_storage__PROCEDURE__TYPE__H

