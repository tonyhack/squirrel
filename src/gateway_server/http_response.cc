//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-10 20:20:59.
// File name: http_response.cc
//
// Description: 
// Define class HttpResponse.
//

#include "gateway_server/http_response.h"

namespace squirrel {

namespace gateway {

const char *HttpResponse::krn_str = "\r\n";
const size_t HttpResponse::krn_size = 2;
const char *HttpResponse::kclose_connection_str = "Connection: close\r\n";
const size_t HttpResponse::kclose_connection_size = 19;
const char *HttpResponse::kkeep_alive_str = "Connection: Keep-Alive\r\n";
const size_t HttpResponse::kkeep_alive_size = 24;

}  // namespace gateway

}  // namespace squirrel

