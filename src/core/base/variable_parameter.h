//
// Summary: server core source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-08-27 15:55:12.
// File name: variable_parameter.h
//
// Description: 
// Micro GET_VARIABLE_PARAMETERS is designed to dispose the variable parameters list,
// build a charactor string using the std-c type va_list.
//

#ifndef __CORE__GET__VARIABLE__PARAMETERS
#define __CORE__GET__VARIABLE__PARAMETERS

#include <stdarg.h>

namespace core {

#define GET_VARIABLE_PARAMETERS(str, str_len, pat)    \
  do{	\
    va_list ap;     \
    bzero(str, str_len);     \
    va_start(ap, pat);      \
    vsnprintf(str, str_len - 1, pat, ap);    \
    va_end(ap);     \
  }while(false)

}  // namespace core


#endif	// __CORE__GET__VARIABLE__PARAMETERS

