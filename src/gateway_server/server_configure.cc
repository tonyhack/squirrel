//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-10 20:20:12.
// File name: server_configure.cc
//
// Description: 
// Define class ServerConfigure.
//

#include "gateway_server/server_configure.h"

#include <iostream>

#include "common/server_type_types.h"
#include "core/base/logging.h"
#include "core/tinyxml/tinyxml.h"
#include "data_proxy_server/client/default_configure.h"

namespace squirrel {

namespace gateway {

ServerConfigure::ServerConfigure(uint32 id) : id_(id) {}
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

  // gateway node.
  TiXmlElement *gateway_node = configure_node->FirstChildElement("gateway_server");
  if(gateway_node == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get node [configure-gateway_server], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, file);
    return false;
  }

  char node[16];
  snprintf(node, sizeof(node), "node%d", this->id_);

  TiXmlElement *node_node = gateway_node->FirstChildElement(node);
  if(node_node == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get node [configure-gateway_server-%s], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  str = node_node->Attribute("ip");
  if(str == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-gateway_server-%s-ip], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }
  this->ip_ = str;

  if(node_node->Attribute("port", (int *)(&this->port_)) == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-gateway_server-%s-port], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  if(node_node->Attribute("thread_number", (int *)(&this->thread_number_)) == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-gateway_server-%s-thread_number], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  str = node_node->Attribute("log");
  if(str == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-gateway_server-%s-log], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }
  this->log_ = str;

  str = node_node->Attribute("core_configure");
  if(str == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-gateway_server-%s-core_configure], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }
  this->core_configure_ = str;

  str = node_node->Attribute("core_log");
  if(str == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-gateway_server-%s-core_log], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }
  this->core_log_ = str;

  TiXmlElement *default_schedule_node = node_node->FirstChildElement("default_schedule");
  if(default_schedule_node == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get node [configure-gateway_server-%s-default_schedule], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  if(default_schedule_node->Attribute("id", (int *)(&this->default_schedule_id_)) == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-gateway_server-%s-default_schedule-id], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  str = default_schedule_node->Attribute("ip");
  if(str == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-gateway_server-%s-default_schedule-ip], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }
  this->default_schedule_ip_ = str;

  if(default_schedule_node->Attribute("port", (int *)(&this->default_schedule_port_)) == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-gateway_server-%s-default_schedule-port], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  CoreLog(INFO, "core configure [%s]", this->core_configure_.c_str());

  // dataproxy_client node.
  TiXmlElement *dataproxy_client_node = configure_node->FirstChildElement("dataproxy_client");
  if(dataproxy_client_node == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get node [configure-dataproxy_client], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, file);
    return false;
  }

  // client_node.
  TiXmlElement *client_node = dataproxy_client_node->FirstChildElement("node");
  while(client_node) {
    common::ServerSignature signature;
    if(client_node->Attribute("id", (int *)(&signature.id_)) == NULL) {
      CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-dataproxy_client-node-id], file(%s).",
          __FILE__, __LINE__, __FUNCTION__, file);
      return false;
    }

    str = client_node->Attribute("ip");
    if(str == NULL) {
      CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-dataproxy_client-node-ip], file(%s).",
          __FILE__, __LINE__, __FUNCTION__, file);
      return false;
    }
    signature.host_ = str;

    if(client_node->Attribute("port", (int *)(&signature.port_)) == NULL) {
      CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-dataproxy_client-node-port], file(%s).",
          __FILE__, __LINE__, __FUNCTION__, file);
      return false;
    }

    client_node = client_node->NextSiblingElement("node");

    dataproxy::client::DefaultConfigureSingleton::GetDefaultConfigure().Insert(signature);
  }

  return true;
}

}  // namespace gateway

}  // namespace squirrel

