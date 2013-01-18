//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-06 16:38:36.
// File name: elf_hash.h
//
// Description: 
// Define function elf_hash.
// This code is copy from the internet.
//

#ifndef _squirrel_global__ELF__HASH__H
#define _squirrel_global__ELF__HASH__H

namespace squirrel {

namespace global {

static unsigned int elf_hash(const char *key, unsigned int length) { 
  unsigned int h = 0, g; 
  const char *end = key + length; 

  while (key < end) { 
    h = (h << 4) + *key++; 
    if ((g = (h & 0xF0000000))) { 
      h = h ^ (g >> 24); 
      h = h ^ g; 
    } 
  } 
  return h; 
}

}  // namespace global

}  // namespace squirrel

#endif  // _squirrel_global__ELF__HASH__H

