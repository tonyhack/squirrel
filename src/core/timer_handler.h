//
// core - server core source code.
//
// $Rev$
// $Author$
// $Date$
//
// Define class TimerHandler.
//

#ifndef __CORE__TIMER__HANDLER__H
#define __CORE__TIMER__HANDLER__H

#include <map>

#include "core/base/types.h"

namespace core {

class Timer;

class TimerHandler {
 public:
  typedef std::map<uint32, Timer *> HandlerTimerList;

  virtual ~TimerHandler() {}
  
  virtual void OnTimer(uint32 id) = 0;
  
  HandlerTimerList timer_list_;
};

}  // namespace core

#endif  // __CORE__TIMER__HANDLER__H

