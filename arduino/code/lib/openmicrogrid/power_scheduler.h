#ifndef POWER_SCHEDULER_H
#define POWER_SCHEDULER_H

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif
#include "grid_message.h"
#include "linked_list.h"      
#include "abstract_ui.h"

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

    void print_me(AbstractUi* ui) {
        switch(type) {
            case P12V_OFF:
                ui->display(F("12V OFF @ %d"), e_time);
                break;
            case P12V_ON:
                ui->display(F("12V ON @ %d"), e_time);
                break;
            case P5V_OFF:
                ui->display(F("5V OFF @ %d"), e_time);
                break;
            case P5V_ON:
                ui->display(F("5V ON @ %d"), e_time);
                break;
            default:
                Serial.print(F("unknown type:"));
                Serial.print(type);
                break;
        }
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

    void print_queue(AbstractUi*);
};


#endif