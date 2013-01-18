//
// Summary: server core source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-08-27 15:44:39.
// File name: event_pipe.h
//
// Description: 
// Define of EventPipe.
//

#ifndef __CORE__EVENT__PIPE__H
#define __CORE__EVENT__PIPE__H

#include "core/version.h"

#ifdef __CORE_KERNEL_EVENTFD__
#include <sys/eventfd.h>
#include <fcntl.h>
#else  // __CORE_KERNEL_EVENTFD__
#include <unistd.h>
#endif  // __CORE_KERNEL_EVENTFD__

namespace core {

inline void CreateEventPipe(int fds[2]) {
#ifdef __CORE_KERNEL_EVENTFD__
  int fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if(fd < 0) {
    CoreLog(FATAL, "%s:%d (%s) Failed in EventPipe",
        __FILE__, __LINE__, __FUNCTION__);
    abort();
  }
  fds[0] = fds[1] = fd;
#else  // __CORE_KERNEL_EVENTFD__
  if(pipe(fds) < 0) {
    CoreLog(FATAL, "%s:%d (%s) Failed in EventPipe",
        __FILE__, __LINE__, __FUNCTION__);
    abort();
  }
#endif  // __CORE_KERNEL_EVENTFD__
}

inline void CloseEventPipe(int fds[2]) {
  close(fds[0]);
#ifndef __CORE_KERNEL_EVENTFD__
  close(fds[1]);
#endif  // __CORE_KERNEL_EVENTFD__
}

inline int EventPipeReadfd(int fds[2]) { return fds[0]; }
inline int EventPipeWritefd(int fds[2]) { return fds[1]; }

}  // namespace core

#endif  // __CORE__EVENT__PIPE__H

