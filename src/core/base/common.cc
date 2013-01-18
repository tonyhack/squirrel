//
// Summary: server core source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-08-27 15:52:20.
// File name: common.cc
//
// Description: 
// common define.
//

#include "core/base/common.h"

#include <ctype.h>

namespace core {

char *strupr(char *str) { 
  char *ptr = str; 

  while (*ptr != '\0') { 
    if (islower(*ptr))  
      *ptr = toupper(*ptr); 
    ptr++; 
  } 

  return str; 
} 

}  // namespace core

