#ifndef POWER_SCHEDULER_H
#define POWER_SCHEDULER_H

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif
#include "grid_message.h"
      
class PowerScheduler {
  public:
    bool schedule(const PowerRequestMessage& msg, uint8_t* when);
};


#endif