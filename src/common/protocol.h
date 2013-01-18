//
// common source code.
// $Rev$
// $Author$
// $Date$
//
// Define protocol.
//

#ifndef _squirrel_common__PROTOCOL__H
#define _squirrel_common__PROTOCOL__H

#include "core/base/types.h"
#include "core/base/byte_buffer_dynamic.h"

using namespace core;

namespace squirrel {

namespace common {

typedef uint32 ProtocolHead;

static const uint32 kProtocolHeadSize = sizeof(ProtocolHead);
static const uint32 kProtocolSizeMask = 0xFFFFFF;

inline const ProtocolHead *ProtocolHeadCast(const char *protocol) {
  return (ProtocolHead *)protocol;
}

inline ProtocolHead *ProtocolHeadCast(char *protocol) {
  return (ProtocolHead *)protocol;
}

inline void SetProtocolHead(char *protocol, ProtocolHead head) {
  *ProtocolHeadCast(protocol) = head;
}

inline char *GetProtocolData(char *protocol) {
  return protocol + kProtocolHeadSize;
}

inline const char *GetProtocolData(const char *protocol) {
  return protocol + kProtocolHeadSize;
}

inline uint32 GetProtocolSize(const char *protocol) {
  return ((*ProtocolHeadCast(protocol) & kProtocolSizeMask) + kProtocolHeadSize);
}
/*
inline const char *Message(core::ByteBufferDynamic &buffer){
  return NULL;
}
*/

}  // namespace common

}  // namespace squirrel

#endif  // _squirrel_common__PROTOCOL__H

