#ifndef GRID_UTILS_H
#define GRID_UTILS_H

#include <EEPROM.h>

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif
#include "abstract_ui.h"


namespace GridUtils {
    template <class T> int EEPROM_write(int ee, const T& value)
    {
        const byte* p = (const byte*)(const void*)&value;
        unsigned int i;
        for (i = 0; i < sizeof(value); i++)
              EEPROM.write(ee++, *p++);
        return i;
    }

    template <class T> int EEPROM_read(int ee, T& value)
    {
        byte* p = (byte*)(void*)&value;
        unsigned int i;
        for (i = 0; i < sizeof(value); i++)
              *p++ = EEPROM.read(ee++);
        return i;
    }

    /*static inline bool is_whitespace(const char& c) {
        return (c == '\r' || c == ' ' || c == '\n');
    }

    static inline bool is_digit(const char& c) {
        return '0'<=c && c<='9';
    }

    static bool parseInt(int * const res) {
        *res = 0;
        bool started = false;
        bool negative = false;
        while(true) {
            int byte = -1;
            while(byte == -1) {
                byte = Serial.read();
            }
            Serial.print((char)byte);

            if (!started && byte == '-') {
                negative = true;
                started = true;
            } else if (!started && is_whitespace((char)byte)) {
                // nothing
            } else if (started && is_whitespace((char)byte)) {
                if (byte != '\n')
                    Serial.println();
                if (negative)
                    *res = -(*res);
                return true;
            } else if (is_digit((char)byte)) {
                started = true;
                int32_t candidate = ((int32_t)*res * 10) + (byte - '0');
                if (negative)
                    candidate = -candidate;
                if (candidate != (int16_t)candidate) {
                    Serial.println();
                    return false;
                }
                if (negative)
                    candidate = -candidate;
                *res = candidate;
            } else {  // got garbage
                Serial.println();
                return false;
            }
        }
    }

    static int get_int(const __FlashStringHelper* msg, bool (*validate)(const int&)) {
        while (true) {
            Serial.print(msg);
            int option = 0;
            if(!parseInt(&option)) {
                Serial.println("Please enter a number.");
                continue;
            }
            if (validate(option)) {
                return option;
            }
        }
    }

    static bool validate01(const int& option) {
        if (option == 0 || option == 1) {
            return true;
        } else {
            Serial.println("Allowed values are 0 or 1.");
            return false;
        }
    }
    static bool validate_byte(const int& option) {
        if (option < 0) {
            Serial.println("Number may not be negative");
            return false;
        } else if (option > 255) {
            Serial.println("Number may not be greater than 255.");
            return false;
        } else {
            return true;
        }
    }*/
};


#endif
