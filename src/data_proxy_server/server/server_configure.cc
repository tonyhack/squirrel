//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-03 17:40:53.
// File name: server_configure.cc
//
// Description: 
// Define class ServerConfigure.
//

#include "data_proxy_server/server/server_configure.h"

#include "core/base/logging.h"
#include "core/tinyxml/tinyxml.h"

using namespace core;

namespace squirrel {

namespace dataproxy {

namespace server {

ServerConfigure::ServerConfigure(core::uint32 id) : id_(id) {}
ServerConfigure::~ServerConfigure() {}

bool ServerConfigure::Load(const char *file) {
  TiXmlDocument doc;
  if(doc.LoadFile(file) == false) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Load file(%s) error.",
        __FILE__, __LINE__, __FUNCTION__, file);
    return false;
  }

  const char *str = NULL;

  // configure node.
  TiXmlElement *configure_node = doc.FirstChildElement("configure");
  if(configure_node == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get node [configure], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, file);
    return false;
  }

  // dataproxy node.
  TiXmlElement *dataproxy_node = configure_node->FirstChildElement("dataproxy_server");
  if(dataproxy_node == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get node [configure-dataproxy_server], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, file);
    return false;
  }

  char node[16];
  snprintf(node, sizeof(node), "node%d", this->id_);

  TiXmlElement *node_node = dataproxy_node->FirstChildElement(node);
  if(node_node == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get node [configure-dataproxy_server-%s], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  TiXmlElement *host_node = node_node->FirstChildElement("host");
  if(host_node == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get node [configure-dataproxy_server-%s-host], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  str = host_node->Attribute("ip");
  if(str == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-dataproxy_server-%s-host-ip], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }
  this->ip_ = str;

  if(host_node->Attribute("port", (int *)(&this->port_)) == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-dataproxy_server-%s-host-port], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  TiXmlElement *thread_number_node = node_node->FirstChildElement("thread_number");
  if(thread_number_node == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get node [configure-dataproxy_server-%s-thread_number], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  if(thread_number_node->Attribute("message", (int *)(&this->message_thread_number_)) == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-dataproxy_server-%s-thread_number-message], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  if(thread_number_node->Attribute("read", (int *)(&this->read_thread_number_)) == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-dataproxy_server-%s-thread_number-read]"
        ", file(%s).", __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  if(thread_number_node->Attribute("write", (int *)(&this->write_thread_number_)) == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-dataproxy_server-%s-thread_number-write]"
        ", file(%s).", __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  TiXmlElement *log_node = node_node->FirstChildElement("log");
  if(log_node == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get node [configure-dataproxy_server-%s-log], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  str = log_node->Attribute("value");
  if(str == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-dataproxy_server-%s-log-value], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }
  this->log_ = str;

  TiXmlElement *core_configure_node = node_node->FirstChildElement("core_configure");
  if(core_configure_node == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get node [configure-dataproxy_server-%s-core_configure], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  str = core_configure_node->Attribute("value");
  if(str == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-dataproxy_server-%s-core_configure-value], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }
  this->core_configure_ = str;

  TiXmlElement *core_log_node = node_node->FirstChildElement("core_log");
  if(core_log_node == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get node [configure-dataproxy_server-%s-core_log], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  str = core_log_node->Attribute("value");
  if(str == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-dataproxy_server-%s-core_log-value], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }
  this->core_log_ = str;

  TiXmlElement *default_schedule_node = node_node->FirstChildElement("default_schedule");
  if(default_schedule_node == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get node [configure-dataproxy_server-%s-default_schedule], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  if(default_schedule_node->Attribute("id", (int *)(&this->default_schedule_id_)) == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-dataproxy_server-%s-default_schedule-id], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  str = default_schedule_node->Attribute("ip");
  if(str == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-dataproxy_server-%s-default_schedule-ip], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }
  this->default_schedule_ip_ = str;

  if(default_schedule_node->Attribute("port", (int *)(&this->default_schedule_port_)) == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-dataproxy_server-%s-default_schedule-port], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  TiXmlElement *request_service_node = node_node->FirstChildElement("request_service");
  if(request_service_node == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get node [configure-dataproxy_server-%s-request_service], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  if(request_service_node->Attribute("max_buffer_size", (int *)(&this->max_buffer_size_)) == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-dataproxy_server-%s-request_service-max_buffer_size], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  // redis.
  TiXmlElement *redis_node = node_node->FirstChildElement("redis");
  if(redis_node == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get node [configure-dataproxy_server-%s-redis], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  core::uint16 redis_port;

  TiXmlElement *redis_node_node = redis_node->FirstChildElement("node");
  while(redis_node_node) {
    str = redis_node_node->Attribute("ip");
    if(str == NULL) {
      CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-dataproxy_server-%s-redis-node-ip], file(%s).",
          __FILE__, __LINE__, __FUNCTION__, node, file);
      return false;
    }
    if(redis_node_node->Attribute("port", (int *)(&redis_port)) == NULL) {
      CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-dataproxy_server-%s-redis-node-port], file(%s).",
          __FILE__, __LINE__, __FUNCTION__, node, file);
      return false;
    }
    this->redis_configure_.AddConnection(std::string(str), redis_port);

    redis_node_node = redis_node_node->NextSiblingElement("node");
  }


  // storage.
  TiXmlElement *storage_node = node_node->FirstChildElement("storage");
  if(storage_node == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get node [configure-dataproxy_server-%s-storage], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  TiXmlElement *storage_node_node = storage_node->FirstChildElement("node");
  while(storage_node_node) {
    storage::Connections storage_connection;
    str = storage_node_node->Attribute("ip");
    if(str == NULL) {
      CoreLog(ERROR, "%s:%d (%s) [ERROR] can't get attribute [configure-dataproxy_server-%s-storage-node-ip], file(%s).",
          __FILE__, __LINE__, __FUNCTION__, node, file);
      return false;
    }
    storage_connection.host_ = str;

    if(storage_node_node->Attribute("port", (int *)(&storage_connection.port_)) == NULL) {
      CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-dataproxy_server-%s-storage-node-port], file(%s).",
          __FILE__, __LINE__, __FUNCTION__, node, file);
      return false;
    }

    str = storage_node_node->Attribute("user");
    if(str == NULL) {
      CoreLog(ERROR, "%s:%d (%s) [ERROR] can't get attribute [configure-dataproxy_server-%s-storage-node-user], file(%s).",
          __FILE__, __LINE__, __FUNCTION__, node, file);
      return false;
    }
    storage_connection.user_ = str;

    str = storage_node_node->Attribute("passwd");
    if(str == NULL) {
      CoreLog(ERROR, "%s:%d (%s) [ERROR] can't get attribute [configure-dataproxy_server-%s-storage-node-passwd], file(%s).",
          __FILE__, __LINE__, __FUNCTION__, node, file);
      return false;
    }
    storage_connection.passwd_ = str;

    str = storage_node_node->Attribute("db");
    if(str == NULL) {
      CoreLog(ERROR, "%s:%d (%s) [ERROR] can't get attribute [configure-dataproxy_server-%s-storage-node-db], file(%s).",
          __FILE__, __LINE__, __FUNCTION__, node, file);
      return false;
    }
    storage_connection.db_ = str;

    this->storage_configure_.AddConnection(storage_connection);

    storage_node_node = storage_node_node->NextSiblingElement("node");
  }


  // procedure.
  TiXmlElement *procedure_node = node_node->FirstChildElement("procedure");
  if(procedure_node == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get node [configure-dataproxy_server-%s-procedure], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  TiXmlElement *procedure_node_node = procedure_node->FirstChildElement("node");
  while(procedure_node_node) {
    core::uint16 procedure_type;
    std::string procedure_name;
    str = procedure_node_node->Attribute("name");
    if(str == NULL) {
      CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-dataproxy_server-%s-procedure-node-name], file(%s).",
          __FILE__, __LINE__, __FUNCTION__, node, file);
      return false;
    }
    procedure_name = str;
    if(procedure_node_node->Attribute("type", (int *)(&procedure_type)) == NULL) {
      CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-dataproxy_server-%s-procedure-node-type], file(%s).",
          __FILE__, __LINE__, __FUNCTION__, node, file);
      return false;
    }
    this->procedure_configure_.Insert((storage::ProcedureType::type)procedure_type, procedure_name);

    procedure_node_node = procedure_node_node->NextSiblingElement("node");
  }

  return true;
}

}  // namespace server

}  // namespace dataproxy

}  // namespace squirrel
