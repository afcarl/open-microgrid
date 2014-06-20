#ifndef POWER_SCHEDULER_H
#define POWER_SCHEDULER_H

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif
#include "grid_message.h"
#include "LinkedList.h"
      

struct PowerEvent {
    typedef enum {
        P12V_OFF =1,
        P12V_ON,
        P5V_OFF,
        P5V_ON,
        DUMMY

    } event_type;

    event_type type;
    long e_time;

    PowerEvent(event_type _type, long _e_time) : type(_type), e_time(_e_time) {
    }

    PowerEvent() : type(DUMMY), e_time(0) {
    } 
};

class PowerScheduler {
  private:
    LinkedList<PowerEvent> events;
  public:
    bool schedule(const PowerRequestMessage& msg, uint8_t* when);

    void push(PowerEvent e);
    bool available();
    PowerEvent pop();

};


#endif