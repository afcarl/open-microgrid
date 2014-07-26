#include "async_lcd_ui.h"


AsyncLcdUi::AsyncLcdUi() : lcd(LiquidCrystal(12, 11, 5, 4, 3, 2)), ctr(0) {
}


void AsyncLcdUi::setup() {
    lcd.begin(16,2);
}

void AsyncLcdUi::display(const __FlashStringHelper* data, ...) {
    if ((int)pgm_read_byte_near((char*)data) == (int)'X') {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("5V @ ");
            // U = IR
            long reading = 0;
            for (int i=0; i<20; ++i) {
                reading += analogRead(A3);
                delay(5);
            }            
            lcd.print(0.51*reading*5/1024/10);
            lcd.print("A");
            lcd.setCursor(0,1);
            lcd.print("12V @ ");
            reading = 0;
            for (int i=0; i<20; ++i) {
                reading += analogRead(A4);
                delay(5);
            }            
            lcd.print(0.51*reading*5/1024/10);            
            lcd.print("A");
            return;
    }

    va_list args;
    va_start(args, data);
    lcd.clear();
    lcd.autoscroll();
    lcd.setCursor(14,0);

    uint8_t index = 0;
    int counter = 0;
    while(true) {
        char chr = pgm_read_byte_near((char*)data + index);
        if (chr == '\0') {
            break;
        } else if (chr == '%') {
            ++index;
            int n;
            chr = pgm_read_byte_near((char*)data + index);
            switch(chr) {
                case 'd':
                    n = va_arg( args, int );
                    lcd.print(n);
                    break;
                case 'D':
                    n = va_arg(args, long);
                    lcd.print(n);
                    break;
                default:
                    lcd.print("<error>");
                    break;
            }
        } else {
            lcd.print(chr);
            if (counter > 20 && chr == ' ') {
                lcd.noAutoscroll();
                delay(500);
                lcd.clear();
                lcd.setCursor(14,0);
                lcd.autoscroll();
                counter = 0;
            }
        }
        ++index;
        counter++;
        delay(100);
    }
    delay(500);
    lcd.noAutoscroll();
    
}

int AsyncLcdUi::get_binary(const __FlashStringHelper* short_text, OneWireKeypad* keypad) {
    int result = -1;
    while(keypad->get_key() != 0)
        delay(20);
    while(result < 0) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(short_text);
        lcd.setCursor(0,1);
        lcd.blink();

        result = -1;
        while (result == -1) {
            uint8_t key = keypad->get_key();
            if( key > 0) {
                switch(key) {
                    case 11:
                        key = 0;
                    case 1:
                        lcd.print((char)('0' + key));
                        delay(500);
                        result = key;
                        break;
                    default:
                        lcd.noBlink();
                        lcd.print(F("Press 0 or 1."));
                        delay(1500);
                        result = -2;
                        break;
                }
            }
        }
    }
    lcd.noBlink();
    return result;
}

int AsyncLcdUi::get_integer(const __FlashStringHelper* short_text, OneWireKeypad* keypad) {
    int result = -2;
    while(keypad->get_key() != 0)
        delay(20);
    while(result < 0) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(short_text);
        lcd.setCursor(0,1);
        lcd.blink();
        int num_so_far = 0;
        result = -1;
        while (result == -1) {
            uint8_t key = keypad->get_key();
            if( key > 0) {
                if (key == 10) { // *
                    // do nothing
                } else if (key == 12) {// #
                    result = num_so_far;
                } else { //digit
                    if (key == 11) key = 0;
                    if (num_so_far == 0 && key == 0) {
                        continue;
                    }
                    lcd.print(key);

                    delay(100);
                    long new_number = (long)num_so_far*10L + (long)key;
                    if (new_number != (int)new_number) {
                        lcd.noBlink();
                        lcd.setCursor(0,1);
                        lcd.print(F("Too long."));
                        delay(1000);
                        result = -2;
                    } else {
                        num_so_far = new_number;
                        while(keypad->get_key() != 0);
                    }
                }
            }
        }
    } 
    lcd.noBlink();

    return result;
}

void AsyncLcdUi::step() {
}