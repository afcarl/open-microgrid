#ifndef ONE_WIRE_KEYPAD_H
#define ONE_WIRE_KEYPAD_H

#include "EEPROM.h"
#include "LiquidCrystal.h"

#define NUM_KEYS 12


const char KEYS[]= {
  '1','2','3',//'A',
  '4','5','6',//'B',
  '7','8','9',//'C',
  '*','0','#',//'D'
};

template <class T> int EEPROM_read(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          *p++ = EEPROM.read(ee++);
    return i;
}

template <class T> int EEPROM_write(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          EEPROM.write(ee++, *p++);
    return i;
}

class OneWireKeypad {
    int16_t * key_to_reading;
    int debounce_time_ms;

  public:
    OneWireKeypad() : debounce_time_ms(30) {
        key_to_reading = new int16_t[13];
    }

    void old_init() {
        int16_t on_stack[] = { 0, 1001, 505, 333, 191, 160, 138, 104, 94, 86, 72, 67, 62 };
        for (int i=0; i<=12; ++i) { key_to_reading[i]=on_stack[i]; }
    }

    void init_from_eeprom(int address) {
        key_to_reading[0] = 0;
        for(int i=0; i<12; ++i) {
            uint16_t value;
            EEPROM_read(address+2*i, value);
            /*lcd.clear();
            lcd.home();
            lcd.print(i+1);
            lcd.print(':');
            lcd.print()*/
            key_to_reading[i+1] = value;
        }
    }

    

    void update_map(uint8_t key, int16_t reading) {
        key_to_reading[key] = reading;
    }

    // 1 2 3 4 5 6 7 8 9 * 0 #

    uint8_t get_key_from_reading(int16_t reading) {
        uint8_t key = 0; // no key
        for (int i = 0; i <= NUM_KEYS; ++i) {
            if (abs(reading - key_to_reading[i]) < abs(reading - key_to_reading[key]) ) {
                key = i;
            }
        }
        return key;
    }

    char char_for_key(uint8_t key) {
        if (1<=key && key<=9) return '0' + key;
        else if (key==10) return '*';
        else if (key==11) return '0';
        else if (key==12) return '#';
    }

    uint8_t get_key() {
        uint16_t reading = analogRead(A1);
        if (reading >= 10) {
            delay(debounce_time_ms);
            uint16_t new_reading = analogRead(A1);
            if (abs(new_reading-reading) < 10) {
                return get_key_from_reading((reading+new_reading)/2);
            } else {
                return 0; // no key
            }
        } else {
            return 0; // no key
        }
    }
    /*uint8_t wait_for_key(long timeout_ms) {
        long start_time = millis();
        while(millis() - start_time < timeout_ms)
        key = get_key();

    }*/

    /*uint8_t get_key() {
        int reading = analogRead(A0);
        if (reading > zero_reading) {
            delay(debounce_time_ms);
            int new_reading = analogRead(A0);
            if (abs(new_reading-reading) < 10) {
                return get_key_from_reading[(reading+new_reading)/2];
            } else {
                return 0; // no key
            }
        } else {
            return 0; // no key
        }
    }*/

    void calibrate(int eeprom_address, LiquidCrystal& lcd) {
        for (int i=0; i < 12; ++i) {
            lcd.clear();
            lcd.home();
            lcd.print("Press ");
            lcd.println(KEYS[i]);
            bool done = false;
            int good_readings = 0;
            int sum_readings = 0;
            while(!done) {
                int reading = analogRead(A1);
                if (reading >= 10 && (good_readings <= 1 || abs(reading - sum_readings/good_readings) < 5)) {
                    ++good_readings;
                    sum_readings += reading;
                    if (good_readings >= 15) {
                        done = true;
                    }
                } else {
                    good_readings = 0;
                    sum_readings = 0;
                }
                delay(20);
            }
            update_map(i+1, sum_readings/good_readings);
            EEPROM_write(eeprom_address+2*i, sum_readings/good_readings);
            lcd.clear();
            lcd.home();
            lcd.print("M ");
            lcd.print(KEYS[i]);
            lcd.print(" to ");
            lcd.println(sum_readings/good_readings);
            delay(1000);
            done = false;
            good_readings = 0;
            while (!done) {
                if (analogRead(A1) >= 10) {
                    good_readings = 0;
                } else {
                    good_readings++;
                    if (good_readings >= 15) {
                        done = true;
                    }
                }
            }
            delay(100);
        }
        lcd.clear();
        lcd.home();
        lcd.println("done");
        delay(1000);
    }


};





#endif