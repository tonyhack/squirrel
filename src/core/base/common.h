//
// Summary: server core source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-08-27 15:52:06.
// File name: common.h
//
// Description: 
// common define.
//

#ifndef __CORE__COMMON__H
#define __CORE__COMMON__H

namespace core {


// ------------------------------------------
// disallow copy and assignment.
//
// to use as the follow example:
//
// class Foo {
//  public:
//   Foo();
//  private:
//   DISALLOW_COPY_AND_ASSIGN(Foo);
// };
// ------------------------------------------
#define DISALLOW_COPY_AND_ASSIGN(TypeName)      \
  TypeName(const TypeName&);                    \
  void operator=(const TypeName&)

// Convert to upper.
char *strupr(char *str);

}  // namespace core

#endif  // __CORE__COMMON__H

