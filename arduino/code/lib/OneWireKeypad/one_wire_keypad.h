#ifndef ONE_WIRE_KEYPAD_H
#define ONE_WIRE_KEYPAD_H

#define NO_KEY '\0'
#define WAITING 0
#define PRESSED 1
#define RELEASED 2
#define HELD 3

#define NUM_KEYS 12

const char KEYS[]= {
  '1','2','3',//'A',
  '4','5','6',//'B',
  '7','8','9',//'C',
  '*','0','#',//'D'
};


class OneWireKeypad {
    int16_t * key_to_reading;
    int debounce_time_ms;

  public:
    OneWireKeypad() : debounce_time_ms(30) {
        key_to_reading = new int16_t[13];
        int16_t on_stack[] = { 0, 1001, 505, 333, 191, 160, 138, 104, 94, 86, 72, 67, 62 };
        for (int i=0; i<=12; ++i) { key_to_reading[i]=on_stack[i]; }
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


};

/*
void calibrate(OneWireKeypad* keypad) {
    Serial.println("Calibration begin");
    for (int i=0; i < 12; ++i) {
        Serial.print("Press ");
        Serial.println(KEYS[i]);
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
        keypad->update_map(i+1, sum_readings/good_readings);
        Serial.print("Mapped ");
        Serial.print(KEYS[i]);
        Serial.print(" to ");
        Serial.println(sum_readings/good_readings);
        Serial.println("Please release key.");

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
    Serial.println("Calibration done");
}
*/


#endif