#ifndef SERIAL_UI_H
#define SERIAL_UI_H

#include <avr/pgmspace.h>
#include <stdarg.h>


#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#include "abstract_ui.h"

class __FlashStringHelper;

class SerialUi : public AbstractUi {
  public:
    void setup() {
        while (!Serial) {
          // Wait for serial connect. Only needed for the Leonardo
          delay(20);
        }
        Serial.setTimeout(100);
    }

    virtual void display(const __FlashStringHelper* data, ...) {
        va_list args;
        va_start(args, data);
        uint8_t index = 0;
        while(true) {
            char chr = pgm_read_byte_near((char*)data + index);
            if (chr == '\0') {
                break;
            } else if (chr == '%') {
                ++index;
                long n;
                chr = pgm_read_byte_near((char*)data + index);
                switch(chr) {
                    case 'd':
                        n = va_arg( args, int );
                        Serial.print(n);
                        break;
                    case 'D':
                        n = va_arg(args, long);
                        Serial.print(n);
                        break;
                    default:
                        Serial.print("<error>");
                        break;
                }
            } else {
                Serial.print(chr);
            }
            ++index;
        }
        Serial.println();
    }


    void step() {

    }
};

#endif