//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-06 15:11:28.
// File name: procedure_configure.h
//
// Description: 
// Define class ProcedureConfigure
//

#ifndef _squirrel_dataproxy_storage__PROCEDURE__CONFIGURE__H
#define _squirrel_dataproxy_storage__PROCEDURE__CONFIGURE__H

#include <map>
#include <string>

#include "data_proxy_server/storage_proxy/procedure_type.h"

namespace squirrel {

namespace dataproxy {

namespace storage {

class ProcedureConfigure : public core::Noncopyable {
  typedef std::map<ProcedureType::type, std::string> ProcedureMap;
 public:
  ProcedureConfigure() {}
  ~ProcedureConfigure() {}

  void Insert(ProcedureType::type type, const std::string &procedure) {
    this->procedures_.insert(std::make_pair(type, procedure));
  }

  bool GetProcedure(ProcedureType::type type, std::string &procedure) const {
    ProcedureMap::const_iterator iterator = this->procedures_.find(type);
    if(iterator != this->procedures_.end()) {
      procedure = iterator->second;
      return true;
    }
    return false;
  }

 private:
  ProcedureMap procedures_;
};

}  // namespace storage

}  // namespace dataproxy

}  // namespace squirrel

#endif  // _squirrel_dataproxy_storage__PROCEDURE__CONFIGURE__H

