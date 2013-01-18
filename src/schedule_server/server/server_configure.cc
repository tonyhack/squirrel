//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-08-28 20:45:03.
// File name: server_configure.cc
//
// Description: 
// Define class ServerConfigure.
//

#include "schedule_server/server/server_configure.h"

#include <iostream>

#include "core/base/logging.h"
#include "core/tinyxml/tinyxml.h"

namespace squirrel {

namespace schedule {

namespace server {

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

  // schedule node.
  TiXmlElement *schedule_node = configure_node->FirstChildElement("schedule_server");
  if(schedule_node == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get node [configure-schedule_server], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, file);
    return false;
  }

  char node[16];
  snprintf(node, sizeof(node), "node%d", this->id_);

  TiXmlElement *node_node = schedule_node->FirstChildElement(node);
  if(node_node == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get node [configure-schedule_server-%s], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  TiXmlElement *host_node = node_node->FirstChildElement("host");
  if(host_node == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get node [configure-schedule_server-%s-host], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  str = host_node->Attribute("ip");
  if(str == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-schedule_server-%s-host-ip], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }
  this->ip_ = str;

  if(host_node->Attribute("port", (int *)(&this->port_)) == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-schedule_server-%s-host-port], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  TiXmlElement *log_node = node_node->FirstChildElement("log");
  if(log_node == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get node [configure-schedule_server-%s-log], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  str = log_node->Attribute("value");
  if(str == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-schedule_server-%s-log-value], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }
  this->log_ = str;

  TiXmlElement *core_configure_node = node_node->FirstChildElement("core_configure");
  if(core_configure_node == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get node [configure-schedule_server-%s-core_configure], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  str = core_configure_node->Attribute("value");
  if(str == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-schedule_server-%s-core_configure-value], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }
  this->core_configure_ = str;

  TiXmlElement *core_log_node = node_node->FirstChildElement("core_log");
  if(core_log_node == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get node [configure-schedule_server-%s-core_log], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  str = core_log_node->Attribute("value");
  if(str == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-schedule_server-%s-core_log-value], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }
  this->core_log_ = str;

  TiXmlElement *default_client_login_node = node_node->FirstChildElement("default_client_login");
  if(default_client_login_node == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get node [configure-schedule_server-%s-default_client_login], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  if(default_client_login_node->Attribute("id", (int *)(&this->default_schedule_id_)) == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-schedule_server-%s-default_client_login-id], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  str = default_client_login_node->Attribute("ip");
  if(str == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-schedule_server-%s-default_client_login-ip], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }
  this->default_schedule_ip_ = str;

  if(default_client_login_node->Attribute("port", (int *)(&this->default_schedule_port_)) == NULL) {
    CoreLog(ERROR, "%s:%d (%s) [ERROR] Can't get attribute [configure-schedule_server-%s-default_client_login-port], file(%s).",
        __FILE__, __LINE__, __FUNCTION__, node, file);
    return false;
  }

  return true;
}

}  // namespace server

}  // namespace schedule

}  // namespace squirrel

