#ifndef ABSTRACT_UI_H
#define ABSTRACT_UI_H

#include <avr/pgmspace.h>
#include <stdarg.h>

#include "one_wire_keypad.h"


class StringArg {
  public:
    virtual uint8_t size();
    virtual char get(uint8_t index);
};

class AbstractUi {
  public:
    virtual void setup();
    virtual void display(const __FlashStringHelper* string_index, ...);
    virtual void step();
    
    virtual int get_binary(const __FlashStringHelper* short_text, OneWireKeypad* keypad) {
        return -1;
    }

    virtual int get_integer(const __FlashStringHelper* short_text, OneWireKeypad* keypad) { 
        return -1;
    }
};

#endif