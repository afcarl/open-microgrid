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

    void print_me() {
        Serial.print("    Event ");
        switch(type) {
            case P12V_OFF:
                Serial.print(F("12V OFF"));
                break;
            case P12V_ON:
                Serial.print(F("12V ON"));
                break;
            case P5V_OFF:
                Serial.print(F("5V OFF"));
                break;
            case P5V_ON:
                Serial.print(F("5V ON"));
                break;
            default:
                Serial.print(F("unknown type:"));
                Serial.print(type);
                break;
        }
        Serial.print(" Happening at ");
        Serial.println(e_time);

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

    void print_queue();
};


#endif