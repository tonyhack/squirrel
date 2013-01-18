//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-06 15:18:54.
// File name: storage.cc
//
// Description: 
// Define class Storage.
//

#include "data_proxy_server/storage_proxy/storage.h"

#include "common/keyvalue_message_types.h"
#include "common/number_message_types.h"
#include "common/list_message_types.h"
#include "common/map_message_types.h"
#include "global/async_logging_singleton.h"
#include "global/elf_hash.h"

namespace squirrel {

using namespace global;

namespace dataproxy {

namespace storage {

Storage::Storage() : connecting_(false), connection_(false), query_(&connection_) {}
Storage::~Storage() {}

bool Storage::Initialize(const std::string &host, core::uint16 port,
    const std::string &user, const std::string &passwd, const std::string &db,
    const ProcedureConfigure &configure) {
  this->db_name_ = db;
  this->host_ = host;
  this->port_ = port;
  this->user_ = user;
  this->passwd_ = passwd;

  size_t pos = ProcedureType::PROCEDURE_KEYVEALUE_SET;
  for(; pos < ProcedureType::PROCEDURE_MAX; ++pos) {
    if(configure.GetProcedure((ProcedureType::type)pos, this->procedure_[pos]) == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Storage] Initialize failed [need procedure %ld].",
          __FILE__, __LINE__, __FUNCTION__, pos);
      return false;
    }
  }
/*
  mysqlpp::Connection *connection = new (std::nothrow) mysqlpp::Connection(false);
  if(connection == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Storage] Allocate msyqlpp::Connection failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  this->connection_.reset(connection);
*/
  return true;
}

bool Storage::Start() {
  mysqlpp::MultiStatementsOption *option1 =
    new (std::nothrow) mysqlpp::MultiStatementsOption(CLIENT_MULTI_STATEMENTS);
  if(option1 == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Storage] Allocate msyqlpp::MultiStatementsOption failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }
  mysqlpp::SetCharsetNameOption *option2 =
    new (std::nothrow) mysqlpp::SetCharsetNameOption("GBK");
  if(option2 == NULL) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Storage] Allocate msyqlpp::SetCharsetNameOption failed.",
        __FILE__, __LINE__, __FUNCTION__);
    return false;
  }

  this->connection_.set_option(option1);
  this->connection_.set_option(option2);

  if(this->connection_.connect(this->db_name_.c_str(),
      this->host_.c_str(), this->user_.c_str(),
      this->passwd_.c_str(), this->port_) == false) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Storage] Connect storage failed [%s:%d, %s, %s:%s] error [%s].",
        __FILE__, __LINE__, __FUNCTION__, this->host_.c_str(), this->port_,
        this->db_name_.c_str(), this->user_.c_str(), this->passwd_.c_str(),
        this->connection_.error());
    this->connecting_ = false;
    return false;
  } else {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
        "[Storage] Connect storage ok [%s:%d, %s, %s:%s].",
        this->host_.c_str(), this->port_, this->db_name_.c_str(),
        this->user_.c_str(), this->passwd_.c_str());
    this->query_ = this->connection_.query();
    this->connecting_ = true;
    return true;
  }
}

bool Storage::Reconnect() {
  if(this->CheckConnectState() == false) {
    if(this->connection_.connected() == true) {
      // Connected, first disconnect it.
      this->connection_.disconnect();
    }
    mysqlpp::MultiStatementsOption *option1 =
      new (std::nothrow) mysqlpp::MultiStatementsOption(CLIENT_MULTI_STATEMENTS);
    if(option1 == NULL) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Storage] Allocate msyqlpp::MultiStatementsOption failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    mysqlpp::SetCharsetNameOption *option2 =
      new (std::nothrow) mysqlpp::SetCharsetNameOption("GBK");
    if(option2 == NULL) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Storage] Allocate msyqlpp::SetCharsetNameOption failed.",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }

    this->connection_.set_option(option1);
    this->connection_.set_option(option2);

    if(this->connection_.connect(this->db_name_.c_str(),
          this->host_.c_str(), this->user_.c_str(),
          this->passwd_.c_str(), this->port_) == false) {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
          "%s:%d (%s) [Storage] Reconnect storage failed [%s:%d, %s, %s:%s] error [%s].",
          __FILE__, __LINE__, __FUNCTION__, this->host_.c_str(), this->port_,
          this->db_name_.c_str(), this->user_.c_str(), this->passwd_.c_str(),
          this->connection_.error());
      this->connecting_ = false;
      return false;
    } else {
      AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_INFO,
          "[Storage] Reconnect storage ok [%s:%d, %s, %s:%s].",
          this->host_.c_str(), this->port_, this->db_name_.c_str(),
          this->user_.c_str(), this->passwd_.c_str());
      this->query_ = this->connection_.query();
      this->connecting_ = true;
      return true;
    }
  }
  return true;
}

int Storage::KeyvalueSet(const std::string &key, const std::string &value) {
  this->ResetQuery();
  this->query_ << "CALL " << this->procedure_[ProcedureType::PROCEDURE_KEYVEALUE_SET]
    << "('" << key << "', " << global::elf_hash(key.data(), key.size())
    << ", '" << mysqlpp::escape << value << "')";
  this->result_ = this->query_.store();
  if(this->query_.errnum() > 0) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Storage] Procedure[%s] failed[%s].",
        __FILE__, __LINE__, __FUNCTION__, this->query_.str().c_str(),
        this->query_.error());
    if(this->Connected() == true) {
      return common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_UNKNOWN;
    } else {
      this->connection_.disconnect();
      this->connecting_ = false;
      return STORAGE_SERIOUS_ERROR;
    }
  } else {
    if(this->result_.num_rows() == 1 && this->result_.num_fields() == 1) {
      if(this->result_.field_name(0) == "@retcode_") {
        if((int)this->result_[0][0] == 0) {
          return common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_SUCCESS;
        } else {
          return common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_UNKNOWN;
        }
      } else {
        return common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_UNKNOWN;
      }
    } else {
      return common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_UNKNOWN;
    }
  }
}

int Storage::KeyvalueGet(const std::string &key, std::string &value) {
  this->ResetQuery();
  this->query_ << "CALL " << this->procedure_[ProcedureType::PROCEDURE_KEYVEALUE_GET]
    << "('" << key << "', " << global::elf_hash(key.data(), key.size())
    << ")";
  this->result_ = this->query_.store();
  if(this->query_.errnum() > 0) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Storage] Procedure[%s] failed[%s].",
        __FILE__, __LINE__, __FUNCTION__, this->query_.str().c_str(),
        this->query_.error());
    if(this->Connected() == true) {
      return common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_UNKNOWN;
    } else {
      this->connection_.disconnect();
      this->connecting_ = false;
      return STORAGE_SERIOUS_ERROR;
    }
  } else {
    if(this->result_.num_rows() == 1 && this->result_.num_fields() == 2) {
      if(this->result_.field_name(0) == "@retcode_") {
        if((int)this->result_[0][0] != 0) {
          return common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_UNKNOWN;
        }
        if(this->result_.field_name(1) == "@value_") {
          value.assign(this->result_[0][1].data(), this->result_[0][1].size());
          return common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_SUCCESS;
        }
      }
      return common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_UNKNOWN;
    } else {
      return common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_UNKNOWN;
    }
  }
}

int Storage::KeyvalueDelete(const std::string &key) {
  this->ResetQuery();
  this->query_ << "CALL " << this->procedure_[ProcedureType::PROCEDURE_KEYVEALUE_DELETE]
    << "('" << key << "', " << global::elf_hash(key.data(), key.size())
    << ")";
  this->result_ = this->query_.store();
  if(this->query_.errnum() > 0) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Storage] Procedure[%s] failed[%s].",
        __FILE__, __LINE__, __FUNCTION__, this->query_.str().c_str(),
        this->query_.error());
    if(this->Connected() == true) {
      return common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_UNKNOWN;
    } else {
      this->connection_.disconnect();
      this->connecting_ = false;
      return STORAGE_SERIOUS_ERROR;
    }
  } else {
    if(this->result_.num_rows() == 1 && this->result_.num_fields() == 1) {
      if(this->result_.field_name(0) == "@retcode_") {
        if((int)this->result_[0][0] == 0) {
          return common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_SUCCESS;
        } else {
          return common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_UNKNOWN;
        }
      } else {
        return common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_UNKNOWN;
      }
    } else {
      return common::KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_UNKNOWN;
    }
  }
}

int Storage::NumberIncrease(const std::string &key, const std::string &increment,
    std::string &value) {
  this->ResetQuery();
  this->query_ << "CALL " << this->procedure_[ProcedureType::PROCEDURE_NUMBER_INCREASE]
    << "('" << key << "', " << global::elf_hash(key.data(), key.size()) << ", '"
    << increment << "')";
  this->result_ = this->query_.store();
  if(this->query_.errnum() > 0) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Storage] Procedure[%s] failed[%s].",
        __FILE__, __LINE__, __FUNCTION__, this->query_.str().c_str(),
        this->query_.error());
    if(this->Connected() == true) {
      return common::NumberResponseType::NUMBER_RESPONSE_TYPE_UNKNOWN;
    } else {
      this->connection_.disconnect();
      this->connecting_ = false;
      return STORAGE_SERIOUS_ERROR;
    }
  } else {
    if(this->result_.num_rows() == 1 && this->result_.num_fields() == 2) {
      if(this->result_.field_name(0) == "@retcode_") {
        if((int)this->result_[0][0] != 0) {
          return common::NumberResponseType::NUMBER_RESPONSE_TYPE_UNKNOWN;
        }
        if(this->result_.field_name(1) == "@value_ret") {
          value.assign(this->result_[0][1].data(), this->result_[0][1].size());
          return common::NumberResponseType::NUMBER_RESPONSE_TYPE_SUCCESS;
        }
        return common::NumberResponseType::NUMBER_RESPONSE_TYPE_UNKNOWN;
      }
      return common::NumberResponseType::NUMBER_RESPONSE_TYPE_UNKNOWN;
    } else {
      return common::NumberResponseType::NUMBER_RESPONSE_TYPE_UNKNOWN;
    }
  }
}

int Storage::NumberDecrease(const std::string &key, const std::string &decrement,
    std::string &value) {
  this->ResetQuery();
  this->query_ << "CALL " << this->procedure_[ProcedureType::PROCEDURE_NUMBER_DECREASE]
    << "('" << key << "', " << global::elf_hash(key.data(), key.size()) << ", '"
    << decrement << "')";
  this->result_ = this->query_.store();
  if(this->query_.errnum() > 0) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Storage] Procedure[%s] failed[%s].",
        __FILE__, __LINE__, __FUNCTION__, this->query_.str().c_str(),
        this->query_.error());
    if(this->Connected() == true) {
      return common::NumberResponseType::NUMBER_RESPONSE_TYPE_UNKNOWN;
    } else {
      this->connection_.disconnect();
      this->connecting_ = false;
      return STORAGE_SERIOUS_ERROR;
    }
  } else {
    if(this->result_.num_rows() == 1 && this->result_.num_fields() == 2) {
      if(this->result_.field_name(0) == "@retcode_") {
        if((int)this->result_[0][0] != 0) {
          return common::NumberResponseType::NUMBER_RESPONSE_TYPE_UNKNOWN;
        }
        if(this->result_.field_name(1) == "@value_ret") {
          value.assign(this->result_[0][1].data(), this->result_[0][1].size());
          return common::NumberResponseType::NUMBER_RESPONSE_TYPE_SUCCESS;
        }
        return common::NumberResponseType::NUMBER_RESPONSE_TYPE_UNKNOWN;
      }
      return common::NumberResponseType::NUMBER_RESPONSE_TYPE_UNKNOWN;
    } else {
      return common::NumberResponseType::NUMBER_RESPONSE_TYPE_UNKNOWN;
    }
  }
}

int Storage::NumberReset(const std::string &key) {
  this->ResetQuery();
  this->query_ << "CALL " << this->procedure_[ProcedureType::PROCEDURE_NUMBER_RESET]
    << "('" << key << "', " << global::elf_hash(key.data(), key.size()) << ")";
  this->result_ = this->query_.store();
  if(this->query_.errnum() > 0) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Storage] Procedure[%s] failed[%s].",
        __FILE__, __LINE__, __FUNCTION__, this->query_.str().c_str(),
        this->query_.error());
    if(this->Connected() == true) {
      return common::NumberResponseType::NUMBER_RESPONSE_TYPE_UNKNOWN;
    } else {
      this->connection_.disconnect();
      this->connecting_ = false;
      return STORAGE_SERIOUS_ERROR;
    }
  } else {
    if(this->result_.num_rows() == 1 && this->result_.num_fields() == 1) {
      if(this->result_.field_name(0) == "@retcode_") {
        if((int)this->result_[0][0] == 0) {
          return common::NumberResponseType::NUMBER_RESPONSE_TYPE_SUCCESS;
        } else {
          return common::NumberResponseType::NUMBER_RESPONSE_TYPE_UNKNOWN;
        }
      }
    }
    return common::NumberResponseType::NUMBER_RESPONSE_TYPE_UNKNOWN;
  }
}

int Storage::NumberDelete(const std::string &key) {
  this->ResetQuery();
  this->query_ << "CALL " << this->procedure_[ProcedureType::PROCEDURE_NUMBER_DELETE]
    << "('" << key << "', " << global::elf_hash(key.data(), key.size()) << ")";
  this->result_ = this->query_.store();
  if(this->query_.errnum() > 0) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Storage] Procedure[%s] failed[%s].",
        __FILE__, __LINE__, __FUNCTION__, this->query_.str().c_str(),
        this->query_.error());
    if(this->Connected() == true) {
      return common::NumberResponseType::NUMBER_RESPONSE_TYPE_UNKNOWN;
    } else {
      this->connection_.disconnect();
      this->connecting_ = false;
      return STORAGE_SERIOUS_ERROR;
    }
  } else {
    if(this->result_.num_rows() == 1 && this->result_.num_fields() == 1) {
      if(this->result_.field_name(0) == "@retcode_") {
        if((int)this->result_[0][0] == 0) {
          return common::NumberResponseType::NUMBER_RESPONSE_TYPE_SUCCESS;
        } else {
          return common::NumberResponseType::NUMBER_RESPONSE_TYPE_UNKNOWN;
        }
      }
    }
    return common::NumberResponseType::NUMBER_RESPONSE_TYPE_UNKNOWN;
  }
}

int Storage::ListPush(const std::string &key, const std::string &element) {
  this->ResetQuery();
  this->query_ << "CALL " << this->procedure_[ProcedureType::PROCEDURE_LIST_PUSH]
    << "('" << key << "', " << global::elf_hash(key.data(), key.size())
    << ", '" << mysqlpp::escape << element << "')";
  this->result_ = this->query_.store();
  if(this->query_.errnum() > 0) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Storage] Procedure[%s] failed[%s].",
        __FILE__, __LINE__, __FUNCTION__, this->query_.str().c_str(),
        this->query_.error());
    if(this->Connected() == true) {
      return common::ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN;
    } else {
      this->connection_.disconnect();
      this->connecting_ = false;
      return STORAGE_SERIOUS_ERROR;
    }
  } else {
    if(this->result_.num_rows() == 1 && this->result_.num_fields() == 1) {
      if(this->result_.field_name(0) == "@retcode_") {
        if((int)this->result_[0][0] == 0) {
          return common::ListResponseType::LIST_RESPONSE_TYPE_SUCCESS;
        } else if((int)this->result_[0][0] == -1){
          return common::ListResponseType::LIST_RESPONSE_TYPE_ELEM_ALREDY_EXIST;
        } else {
          return common::ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN;
        }
      }
    }
    return common::ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN;
  }
}

int Storage::ListPop(const std::string &key, std::string &element) {
  this->ResetQuery();
  this->query_ << "CALL " << this->procedure_[ProcedureType::PROCEDURE_LIST_POP]
    << "('" << key << "', " << global::elf_hash(key.data(), key.size()) << ")";
  this->result_ = this->query_.store();
  if(this->query_.errnum() > 0) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Storage] Procedure[%s] failed[%s].",
        __FILE__, __LINE__, __FUNCTION__, this->query_.str().c_str(),
        this->query_.error());
    if(this->Connected() == true) {
      return common::ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN;
    } else {
      this->connection_.disconnect();
      this->connecting_ = false;
      return STORAGE_SERIOUS_ERROR;
    }
  } else {
    if(this->result_.num_rows() == 1 && this->result_.num_fields() == 2) {
      if(this->result_.field_name(0) == "@retcode_") {
        if((int)this->result_[0][0] != 0) {
          if((int)this->result_[0][0] == -1) {
            return common::ListResponseType::LIST_RESPONSE_TYPE_EMPTY;
          } else {
            return common::ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN;
          }
        }
        if(this->result_.field_name(1) == "@element_") {
          element.assign(this->result_[0][1].data(), this->result_[0][1].size());
          return common::ListResponseType::LIST_RESPONSE_TYPE_SUCCESS;
        }
      }
      return common::ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN;
    } else {
      return common::ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN;
    }
  }
}

int Storage::ListRemove(const std::string &key, const std::string &element) {
  this->ResetQuery();
  this->query_ << "CALL " << this->procedure_[ProcedureType::PROCEDURE_LIST_REMOVE]
    << "('" << key << "', " << global::elf_hash(key.data(), key.size())
    << ", '" << mysqlpp::escape << element << "')";
  this->result_ = this->query_.store();
  if(this->query_.errnum() > 0) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Storage] Procedure[%s] failed[%s].",
        __FILE__, __LINE__, __FUNCTION__, this->query_.str().c_str(),
        this->query_.error());
    if(this->Connected() == true) {
      return common::ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN;
    } else {
      this->connection_.disconnect();
      this->connecting_ = false;
      return STORAGE_SERIOUS_ERROR;
    }
  } else {
    if(this->result_.num_rows() == 1 && this->result_.num_fields() == 1) {
      if(this->result_.field_name(0) == "@retcode_") {
        if((int)this->result_[0][0] == 0) {
          return common::ListResponseType::LIST_RESPONSE_TYPE_SUCCESS;
        } else {
          return common::ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN;
        }
      }
    }
    return common::ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN;
  }
}

int Storage::ListClear(const std::string &key) {
  this->ResetQuery();
  this->query_ << "CALL " << this->procedure_[ProcedureType::PROCEDURE_LIST_CLEAR]
    << "('" << key << "', " << global::elf_hash(key.data(), key.size()) << ")";
  this->result_ = this->query_.store();
  if(this->query_.errnum() > 0) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Storage] Procedure[%s] failed[%s].",
        __FILE__, __LINE__, __FUNCTION__, this->query_.str().c_str(),
        this->query_.error());
    if(this->Connected() == true) {
      return common::ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN;
    } else {
      this->connection_.disconnect();
      this->connecting_ = false;
      return STORAGE_SERIOUS_ERROR;
    }
  } else {
    if(this->result_.num_rows() == 1 && this->result_.num_fields() == 1) {
      if(this->result_.field_name(0) == "@retcode_") {
        if((int)this->result_[0][0] == 0) {
          return common::ListResponseType::LIST_RESPONSE_TYPE_SUCCESS;
        } else if((int)this->result_[0][0] == -1){
          return common::ListResponseType::LIST_RESPONSE_TYPE_KEY_NOT_EXIST;
        } else {
          return common::ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN;
        }
      }
    }
    return common::ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN;
  }
}

int Storage::ListDelete(const std::string &key) {
  this->ResetQuery();
  this->query_ << "CALL " << this->procedure_[ProcedureType::PROCEDURE_LIST_DELETE]
    << "('" << key << "', " << global::elf_hash(key.data(), key.size()) << ")";
  this->result_ = this->query_.store();
  if(this->query_.errnum() > 0) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Storage] Procedure[%s] failed[%s].",
        __FILE__, __LINE__, __FUNCTION__, this->query_.str().c_str(),
        this->query_.error());
    if(this->Connected() == true) {
      return common::ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN;
    } else {
      this->connection_.disconnect();
      this->connecting_ = false;
      return STORAGE_SERIOUS_ERROR;
    }
  } else {
    if(this->result_.num_rows() == 1 && this->result_.num_fields() == 1) {
      if(this->result_.field_name(0) == "@retcode_") {
        if((int)this->result_[0][0] == 0) {
          return common::ListResponseType::LIST_RESPONSE_TYPE_SUCCESS;
        } else if((int)this->result_[0][0] == -1){
          return common::ListResponseType::LIST_RESPONSE_TYPE_KEY_NOT_EXIST;
        } else {
          return common::ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN;
        }
      }
    }
    return common::ListResponseType::LIST_RESPONSE_TYPE_UNKNOWN;
  }
}

int Storage::MapInsert(const std::string &key, const std::string &score, const std::string &element) {
  this->ResetQuery();
  this->query_ << "CALL " << this->procedure_[ProcedureType::PROCEDURE_MAP_INSERT]
    << "('" << key << "', " << global::elf_hash(key.data(), key.size())
    << ",'" << score << "', '" << mysqlpp::escape << element << "')";
  this->result_ = this->query_.store();
  if(this->query_.errnum() > 0) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Storage] Procedure[%s] failed[%s].",
        __FILE__, __LINE__, __FUNCTION__, this->query_.str().c_str(),
        this->query_.error());
    if(this->Connected() == true) {
      return common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
    } else {
      this->connection_.disconnect();
      this->connecting_ = false;
      return STORAGE_SERIOUS_ERROR;
    }
  } else {
    if(this->result_.num_rows() == 1 && this->result_.num_fields() == 1) {
      if(this->result_.field_name(0) == "@retcode_") {
        if((int)this->result_[0][0] == 0) {
          return common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS;
        } else {
          return common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
        }
      }
    }
    return common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
  }
}

int Storage::MapRemove(const std::string &key, const std::string &element) {
  this->ResetQuery();
  this->query_ << "CALL " << this->procedure_[ProcedureType::PROCEDURE_MAP_REMOVE]
    << "('" << key << "', " << global::elf_hash(key.data(), key.size())
    << ", '" << mysqlpp::escape << element << "')";
  this->result_ = this->query_.store();
  if(this->query_.errnum() > 0) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Storage] Procedure[%s] failed[%s].",
        __FILE__, __LINE__, __FUNCTION__, this->query_.str().c_str(),
        this->query_.error());
    if(this->Connected() == true) {
      return common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
    } else {
      this->connection_.disconnect();
      this->connecting_ = false;
      return STORAGE_SERIOUS_ERROR;
    }
  } else {
    if(this->result_.num_rows() == 1 && this->result_.num_fields() == 1) {
      if(this->result_.field_name(0) == "@retcode_") {
        if((int)this->result_[0][0] == 0) {
          return common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS;
        } else if((int)this->result_[0][0] == -1) {
          return common::MapResponseType::MAP_RESPONSE_TYPE_KEY_NOT_EXIST;
        } else {
          return common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
        }
      }
    }
    return common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
  }
}

int Storage::MapQueryRank(const std::string &key, const std::string &element,
    std::string &ranking, bool asc) {
  int integer_asc = asc ? 1 : 0;
  this->ResetQuery();
  this->query_ << "CALL " << this->procedure_[ProcedureType::PROCEDURE_MAP_QUERY_RANK]
    << "('" << key << "', " << global::elf_hash(key.data(), key.size())
    << ", '" << mysqlpp::escape << element << "', " << integer_asc << ")";
  this->result_ = this->query_.store();
  if(this->query_.errnum() > 0) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Storage] Procedure[%s] failed[%s].",
        __FILE__, __LINE__, __FUNCTION__, this->query_.str().c_str(),
        this->query_.error());
    if(this->Connected() == true) {
      return common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
    } else {
      this->connection_.disconnect();
      this->connecting_ = false;
      return STORAGE_SERIOUS_ERROR;
    }
  } else {
    if(this->result_.num_rows() == 1 && this->result_.num_fields() == 2) {
      if(this->result_.field_name(0) == "@retcode_") {
        int retcode = (int)this->result_[0][0];
        if(retcode != 0) {
          if(retcode == -1) {
            return common::MapResponseType::MAP_RESPONSE_TYPE_KEY_NOT_EXIST;
          } else if(retcode == -2) {
            return common::MapResponseType::MAP_RESPONSE_TYPE_ELEM_NOT_EXIST;
          } else {
            return common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
          }
        }
        if(this->result_.field_name(1) == "@ranking_") {
          ranking.assign(this->result_[0][1].data(), this->result_[0][1].size());
          return common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS;
        }
      }
    }
    return common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
  }
}

int Storage::MapQueryRange(const std::string &key, const std::string &rank_start,
    const std::string &rank_end, bool asc) {
  this->ResetQuery();
  int integer_asc = asc ? 1 : 0;
  this->query_ << "CALL " << this->procedure_[ProcedureType::PROCEDURE_MAP_QUERY_RANGE]
    << "('" << key << "', " << global::elf_hash(key.data(), key.size())
    << ", '" << rank_start << "', '" << rank_end << "', " << integer_asc << ")";
  this->result_ = this->query_.store();
  if(this->query_.errnum() > 0) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Storage] Procedure[%s] failed[%s].",
        __FILE__, __LINE__, __FUNCTION__, this->query_.str().c_str(),
        this->query_.error());
    if(this->Connected() == true) {
      return common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
    } else {
      this->connection_.disconnect();
      this->connecting_ = false;
      return STORAGE_SERIOUS_ERROR;
    }
  } else {
    if(this->result_.num_rows() > 0) {
      if(this->result_.field_name(0) == "@retcode_") {
        int retcode = (int)this->result_[0][0];
        if(retcode == -1) {
          return common::MapResponseType::MAP_RESPONSE_TYPE_KEY_NOT_EXIST;
        } else if(retcode == -2) {
          return common::MapResponseType::MAP_RESPONSE_TYPE_ELEM_NOT_EXIST;
        } else {
          return common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
        }
      }
    }
    return common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS;
  }
}

int Storage::MapQueryRangeWithscores(const std::string &key, const std::string &rank_start,
    const std::string &rank_end, bool asc) {
  return this->MapQueryRange(key, rank_start, rank_end, asc);
}

bool Storage::MapFetchRangeValue(std::string &element, size_t index) {
  mysqlpp::StoreQueryResult::list_type::size_type row_number = this->result_.num_rows();
  if(row_number > index) {
    if(this->result_.num_fields() >= 2 && this->result_.field_name(1) == "element") {
      element.assign(this->result_[index][1].data(), this->result_[index][1].size());
      return true;
    }
  }
  return false;
}

bool Storage::MapFetchRangeValueWithscores(std::string &score,
    std::string &element, size_t index) {
  mysqlpp::StoreQueryResult::list_type::size_type row_number = this->result_.num_rows();
  if(row_number > index) {
    if(this->result_.num_fields() >= 2 && this->result_.field_name(0) == "ranking_key" &&
        this->result_.field_name(1) == "element") {
      score.assign(this->result_[index][0].data(), this->result_[index][0].size());
      element.assign(this->result_[index][1].data(), this->result_[index][1].size());
      return true;
    }
  }
  return false;
}

void Storage::MapQuitFetchRange() {
  this->ResetQuery();
}

int Storage::MapResetScore(const std::string &key,
    const std::string &score, const std::string &element) {
  return this->MapInsert(key, score, element);
}

int Storage::MapClear(const std::string &key) {
  this->ResetQuery();
  this->query_ << "CALL " << this->procedure_[ProcedureType::PROCEDURE_MAP_CLEAR]
    << "('" << key << "', " << global::elf_hash(key.data(), key.size()) << ")";
  this->result_ = this->query_.store();
  if(this->query_.errnum() > 0) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Storage] Procedure[%s] failed[%s].",
        __FILE__, __LINE__, __FUNCTION__, this->query_.str().c_str(),
        this->query_.error());
    if(this->Connected() == true) {
      return common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
    } else {
      this->connection_.disconnect();
      this->connecting_ = false;
      return STORAGE_SERIOUS_ERROR;
    }
  } else {
    if(this->result_.num_rows() == 1 && this->result_.num_fields() == 1) {
      if(this->result_.field_name(0) == "@retcode_") {
        if((int)this->result_[0][0] == 0) {
          return common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS;
        } else if((int)this->result_[0][0] == -1){
          return common::MapResponseType::MAP_RESPONSE_TYPE_KEY_NOT_EXIST;
        } else {
          return common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
        }
      }
    }
    return common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
  }
}

int Storage::MapDelete(const std::string &key) {
  this->ResetQuery();
  this->query_ << "CALL " << this->procedure_[ProcedureType::PROCEDURE_MAP_DELETE]
    << "('" << key << "', " << global::elf_hash(key.data(), key.size()) << ")";
  this->result_ = this->query_.store();
  if(this->query_.errnum() > 0) {
    AsyncLoggingSingleton::GetAsyncLogging()->Logging(LOGGING_LEVEL_ERROR,
        "%s:%d (%s) [Storage] Procedure[%s] failed[%s].",
        __FILE__, __LINE__, __FUNCTION__, this->query_.str().c_str(),
        this->query_.error());
    if(this->Connected() == true) {
      return common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
    } else {
      this->connection_.disconnect();
      this->connecting_ = false;
      return STORAGE_SERIOUS_ERROR;
    }
  } else {
    if(this->result_.num_rows() == 1 && this->result_.num_fields() == 1) {
      if(this->result_.field_name(0) == "@retcode_") {
        if((int)this->result_[0][0] == 0) {
          return common::MapResponseType::MAP_RESPONSE_TYPE_SUCCESS;
        } else if((int)this->result_[0][0] == -1){
          return common::MapResponseType::MAP_RESPONSE_TYPE_KEY_NOT_EXIST;
        } else {
          return common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
        }
      }
    }
    return common::MapResponseType::MAP_RESPONSE_TYPE_UNKNOWN;
  }
}

void Storage::ResetQuery() {
  this->query_.reset();
  while(this->query_.more_results()) {
    this->query_.store_next();
  }
}

bool Storage::Connected() {
  return this->connection_.connected() && this->connection_.ping();
}

}  // namespace storage

}  // namespace dataproxy

}  // namespace squirrel

