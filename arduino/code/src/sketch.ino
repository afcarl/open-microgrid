#include <EEPROM.h>
#include <SPI.h>

#define RH_MESH_MAX_MESSAGE_LEN 50
#include "RHMesh.h"
#include "RH_NRF24.h"
#include "Logging.h"
#include "smart_assert.h"
#include "power_client_api.h"
#include "grid_utils.h"
#include "grid_networking.h"
#include "grid_message.h"
#include "power_scheduler.h"
#include "LiquidCrystal.h"
#include "one_wire_keypad.h"

#define PRICE_PER_WATT_SECOND_IN_MILIRUPEES 50

#define EEPROM_MY_NUMBER_ADDRESS 0
#define EEPROM_BALANCE_ADDRESS 1
#define EEPROM_KEYBOARD_CONFIG_ADDRESS 5


// TODOS:
// - screen updating balance too frequently
// - keyboard calibration on EEPROM
// - periodically ask for permission for power. If denied set state to network of 
// and periodically query for reenable. 



// Singleton instance of the radio driver
RH_NRF24 driver(8, 10);
// This code manages mesh network state and actions - maintains packets,
// discovers routes etc. Address will be later updated 
RHMesh manager(driver, 0);

// AbstractUi* ui;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

uint8_t buf[RH_MESH_MAX_MESSAGE_LEN];

GridNetworking network(manager, buf, RH_MESH_MAX_MESSAGE_LEN);

PowerScheduler scheduler;

uint8_t my_number = 0;

OneWireKeypad* keypad;

void set_mode(int m);

void funny_assert(bool statement, const __FlashStringHelper* message = NULL) {
    if (!statement) {
        while(true) {
            lcd.clear();
            lcd.home();
            lcd.print(F("Assertion failed"));
            if(message) {
                lcd.setCursor(0,1);
                lcd.print(message);
            }
            delay(2000);
        }
    }
}

#define MAGIC_NUMBER 0xF3F3F3F3L

class LcdManager {
    const char* top_line;
    const char* bottom_line;
    bool top_progmem, bottom_progmem;
    long butter_start_ms;
    int butter_length_dms;
    int state;

    void _butter(const char* text, int time_ms, long number, bool progmem) {
        state = 1; // butter on
        butter_start_ms = millis();
        butter_length_dms = time_ms;
        lcd.clear();
        lcd.home();
        if (progmem) lcd.print((const __FlashStringHelper*)text);
        else lcd.print(text);
        if (number != MAGIC_NUMBER) {
            lcd.print(number);
        }
    }
    void _set_line(const char* text, bool top, bool progmem) {
        if (top) {
            top_progmem = progmem;
            top_line = text;
        } else {
            bottom_progmem = progmem;
            bottom_line = text;
        }
        if (state == 0) { // butter off
            reset_ui();
        }
    }

  public:
    LcdManager() : top_line(NULL),
                   bottom_line(NULL),
                   state(0) {  // butter off
    }

    void butter(const __FlashStringHelper* text, int time_ms, long number=MAGIC_NUMBER) {
        _butter((const char*)text, time_ms, number, true);
    }

    void butter(const char* text, int time_ms, long number=MAGIC_NUMBER) {
        _butter(text, time_ms, number, false);        
    }

    void reset_ui() {
        lcd.clear();

        lcd.setCursor(0,0);
        if (top_line) {
            if (top_progmem) lcd.print((const __FlashStringHelper*)top_line);
            else lcd.print(top_line);
        } else {
            lcd.print(F(""));
        }

        lcd.setCursor(0,1);
        if (bottom_line) {
            if (bottom_progmem) lcd.print((const __FlashStringHelper*)bottom_line);
            else lcd.print(bottom_line);
        } else {
            lcd.print(F(""));
        }
        state = 0; // butter off
    }

    void set_line(const __FlashStringHelper* text, bool top=true) {
        _set_line((const char*)text, top, true);
    }

    void set_line(const char* text, bool top=true) {
        _set_line(text, top, false);
    }

    void step() {
        if (state == 1 && millis() - butter_start_ms > butter_length_dms) {
            //lcd.clear();
            //lcd.home()
            reset_ui();
        }
    }
};

LcdManager lcd_manager;

#define MONEY_BACKUP_FREQUENCY_MS 15000

class MoneyManager {
    long balance;
    long last_backup;

  private:
    void update_balance(long new_value) {
        balance = new_value;
        if (last_backup == 0) last_backup = millis();
        if (millis() - last_backup >= MONEY_BACKUP_FREQUENCY_MS) {
            GridUtils::EEPROM_write(EEPROM_BALANCE_ADDRESS, balance);
            last_backup = millis();
        }
    }

  public:
    MoneyManager() : last_backup(0) { // 50 rupee
        GridUtils::EEPROM_read(EEPROM_BALANCE_ADDRESS, balance);
    }

    // true if enough balance
    bool subtract_money(long to_substract) {
        funny_assert(to_substract >= 0, F("Negative moni"));
        if (balance - to_substract >= 0) {
            update_balance(balance - to_substract);
            return true;
        }  else {
            return false;
        }
    }

    void add_money(long to_add) {
        update_balance(balance + to_add);
    }

    long get_balance_milirupees() {
        return balance;
    }
};

MoneyManager money_manager;


#define MODE_NOTIFICATIONS 0
#define MODE_KEYPAD_INPUT 1
int mode;


#define UI_UPDATE_FREQUENCY_MS 2000
#define MONEY_UPDATE_FREQUENCY 500

class UiManager {
    // notifications
    char buffer[17];
    long last_message;
    long last_update;
    int message;

    //keypad input
    int next_char;

  public:
    UiManager() {
        on_mode_set(MODE_NOTIFICATIONS);
    }

    // returns ending
    int number_to_buffer(const int& number, const int& at) {
        funny_assert(number>=0, F("lcd negative"));
        int number_copy = number;
        int n_digits = 0;
        while(number_copy) {
            n_digits++;
            number_copy/=10;
        }

        if (number == 0)
            n_digits = 1;
        number_copy = number;

        for(int i=at + n_digits - 1; i>=at; --i) {
            funny_assert(i<16, F("buf overflow"));
            buffer[i]=(char)(number_copy%10 + '0');
            number_copy/=10;
        }
        return at + n_digits;
    }

    void on_mode_set(int m) {
        mode = m;
        switch (mode) {
            case MODE_NOTIFICATIONS:
              lcd.noBlink();
              lcd_manager.set_line(F("Notifications"));
              message = 0;
              last_message = 0;
              break;
            case MODE_KEYPAD_INPUT:
              lcd.blink();
              lcd_manager.set_line(F("Enter code:"));
              next_char = 0;
              lcd_manager.set_line((const char*)NULL, false);

              break;
            default:
              funny_assert(false, F("Wrong mode"));
              break;
              // do something
        }
    }

    int string_to_buffer(const __FlashStringHelper* str, const int& at) {
        int index = 0;
        while(true) {
            funny_assert(at+index < 16);
            char chr = pgm_read_byte_near((char*)str + index);
            if (chr == '\0') break;
            buffer[at+index] = chr;
            ++index;
        }
        return at+index;
    }

    void keypad_display(char x) {
        funny_assert(mode == MODE_KEYPAD_INPUT, F("MODE NOKEY"));
        if (next_char>=16) {
            lcd_manager.butter("Key too long.", 1000);
            set_mode(MODE_NOTIFICATIONS);
        }
        buffer[next_char++] = x;
        buffer[next_char] = 0;
        lcd_manager.set_line(buffer, false);
    }

    void get_entered_key(char** key, int* len) {
        *len = next_char;
        *key = buffer;
    }

    void keypad_input() {
        // do nothing - magic happens in keypad char pressed
    }

    void update_message() {
        switch (mode) {
            case MODE_NOTIFICATIONS:
              notifications();
              break;
            case MODE_KEYPAD_INPUT:
              keypad_input();
              break;
            default:
              funny_assert(false, F("Wrong mode"));
              break;
              // do something
        }
    }

    void notifications() {
        bool new_message;
        if (last_message == 0 || 
                millis() - last_message > (long)UI_UPDATE_FREQUENCY_MS) {   // set new message
            PROGMEM const int num_messages = 3;
            if (last_message == 0) message = 0;
            else message = (message + 1) % num_messages;
            new_message = true;
            last_message = millis();
        } else { // update message
            new_message = false; 
        }
        int end;
        int amt;
        switch (message) {
            case 0:
                if (new_message) {
                    lcd_manager.set_line(F("1 - add money"), false);
                }
                break;
            case 1:
                if (new_message) {
                    lcd_manager.set_line(F("2 - calibr keys"), false);
                }
                break;
            case 2:
                // factor out
                // debug
                if (!last_update) last_update = millis();
                if (millis() - last_update > MONEY_UPDATE_FREQUENCY) {
                    end = 0;
                    end = string_to_buffer(F("B:"), 0);
                    amt = (int)(money_manager.get_balance_milirupees()/1000L);
                    end = number_to_buffer(amt, end);
                    end = string_to_buffer(F(" INR"), end);
                    funny_assert(end <=16);
                    buffer[end] = 0;
                    lcd_manager.set_line(buffer, false);
                    last_update = millis();
                }
                break;
            default:
              // do something
              break;
        }
    

    }
};

UiManager ui_manager;



// currently Arduino connected to rpi acts as a "generator". It's number is 1. Other nodes are
// 0 and 2.
#define GENERATOR_NUMBER 1

// control pins for electricity output. Enabled on LOW.
#define GRID_OUTPUT_12V 7
#define GRID_OUTPUT_5V 6

void client_instructions();

void setup() 
{
    Log.Init(LOG_LEVEL_DEBUG, 57600);

    randomSeed(analogRead(0));

    pinMode(GRID_OUTPUT_12V, OUTPUT);
    pinMode(GRID_OUTPUT_5V, OUTPUT);

    // off on high
    digitalWrite(GRID_OUTPUT_12V, LOW);
    // off on low
    digitalWrite(GRID_OUTPUT_5V, HIGH);

    my_number = EEPROM.read(EEPROM_MY_NUMBER_ADDRESS);
    manager.setThisAddress(my_number);

    assert (manager.init());
    assert (driver.setRF(RH_NRF24::DataRate250kbps, RH_NRF24::TransmitPower0dBm));

    if (my_number == GENERATOR_NUMBER) {
        ///// FIXME ///////////////////////////////////////////////////////////////////////////////
        /*lcd.clear();
        lcd.setCursor(0,0)
        lcd.print(F("Generator mode")); // "Generator Mode."
        */
    } else {
        // TODO: change this to LcdUi when ready.
        //ui = new SerialUi();
        lcd.begin(16,2);
    }
    keypad = new OneWireKeypad();
    keypad->init_from_eeprom(EEPROM_KEYBOARD_CONFIG_ADDRESS);
}

void generator_loop() {
    /*
    // currently timeout is 1000ms for receive.
    GridMessage* message = network.receive_message();
    handle_message(message);
    */
}
/*
void handle_message(GridMessage* message) {
    if (message != NULL) {
        if (message->type == GridMessage::POWER_REQUEST_MESSAGE) {
            ui->display(F("Received power request."));
            message->describe(ui);
            on_power_request(*(PowerRequestMessage*)message);
            delete message;
        }
        // clean up our mess;
        delete message;
    }
}

void on_power_request(const PowerRequestMessage& msg) {
    uint8_t when;
    bool granted = scheduler.schedule(msg, &when);
    PowerResponseMessage resp;
    if (granted) {
        resp.when = when;
        resp.response = PowerResponseMessage::GRANTED;
    } else {
        resp.response = PowerResponseMessage::DENIED;
    }
    network.transmit_message(resp, msg.from);

}

void issue_power_request() {
    ui->display(F("Request for power."));
    PowerRequestMessage res = PowerClientApi::power_request_from_stdin(ui, keypad);
    
    // ui->display(F("You created the following power request:"));
    // res.describe(ui);

    int temp = ui->get_binary(F("Submit? [0/1]:"), keypad);
    
    if (temp == 1) {
        while (true) {
            bool success = false;
            GridMessage* reply;
            if (network.transmit_message(res, GENERATOR_NUMBER)) {
                reply = network.receive_message();
                if (reply != NULL &&
                    reply->type == GridMessage::POWER_RESPONSE_MESSAGE &&
                    reply->from == GENERATOR_NUMBER) {
                    success = true;
                } else {
                    ui->display(F("Wrong or no response."));           
                }
            } else {
                ui->display(F("Could not transmit message."));
            }
            if (!success) {
                if (reply != NULL) 
                    delete reply;
                temp = ui->get_binary(F("Submit? [0/1]:"), keypad);

                if (temp == 0)  break;
            } else {
                reply->describe(ui);
                PowerResponseMessage* response = (PowerResponseMessage*)reply;
                if (response->response == PowerResponseMessage::GRANTED) {
                    long delay = millis() + (long)1000*(long)response->when;

                    if (res.flags & (1<<1)) {  // 12V
                        scheduler.push(PowerEvent(PowerEvent::P12V_ON, delay));
                        scheduler.push(PowerEvent(PowerEvent::P12V_OFF, delay+(long)1000*(long)res.duration));
                    } else {  // 5V 
                        scheduler.push(PowerEvent(PowerEvent::P5V_ON, delay));
                        scheduler.push(PowerEvent(PowerEvent::P5V_OFF, delay+(long)1000*(long)res.duration));                       
                    }
                }
                delete reply;
                break;
            }
        }
    } else {
        ui->display(F("Request canceled."));
    }
}

void keypad_step() {
    uint8_t key = keypad->get_key();

    switch (key) {
         case 1:
           scheduler.print_queue(ui);
           break;
         case 2:
           issue_power_request();

           break;
         default:
           delay(20);
           break;
    } 
}
*/
#define NUM_OUTLETS 2
#define OUTLET_5V 0
#define OUTLET_12V 1

#define POWER_OFF 0
#define POWER_ON 1

int state[2];



class Power {
    int current_ma[2];
    long last_money_update;

    long overdue_amt;
    bool overdue;
  public:
    Power() : last_money_update(0),
              overdue(false) {
    }
    void sense() {
        // U = IR
        for (int outlet=0; outlet < NUM_OUTLETS; ++outlet) {
            long reading = 0;
            for (int i=0; i<20; ++i) {
                reading += analogRead( (outlet == OUTLET_5V) ? A3 : A4);
                delay(2);
            }
            // current in mA = reading / 1024 * 5V / (1.1 ohm) / (10 readings) * 1000 
            current_ma[outlet] = reading*1000L*5L/1024L/11;   
        }
    }

    void actions_on_lcd() {
        for (int outlet=0; outlet < NUM_OUTLETS; ++outlet) {
            if (current_ma[outlet] < 50 && state[outlet] == POWER_ON) {
                state[outlet] = POWER_OFF;
                lcd_manager.butter((outlet == OUTLET_5V) ? F("5V OFF") : F("12V OFF"), 1000);
            } else if (current_ma[outlet] >= 50 && state[outlet] == POWER_OFF) {
                state[outlet] = POWER_ON;
                lcd.clear();
                lcd.setCursor(0,0);
                lcd_manager.butter((outlet == OUTLET_5V) ? F("5V ON") : F("12V ON"), 1000);
            }
        }
    }

    bool ensure_no_overdue() {
        if (overdue && money_manager.subtract_money(overdue_amt)) {
                overdue = false;
        }
        return !overdue;
    }

    void power_off() {
        // off on high
        digitalWrite(GRID_OUTPUT_12V, HIGH);
        // off on low
        digitalWrite(GRID_OUTPUT_5V, LOW);
    }

    void power_on() {
        // off on high
        digitalWrite(GRID_OUTPUT_12V, LOW);
        // off on low
        digitalWrite(GRID_OUTPUT_5V, HIGH);
    }

    void update_money() {
        if(!ensure_no_overdue()) {
            power_off();
        } else {
            power_on();
        }
        if (last_money_update == 0)
            last_money_update = millis();
        long price_per_s = 0;
        long power_miliwatts = 0;
        for (int outlet=0; outlet < NUM_OUTLETS; ++outlet) {
            if (state[outlet] == POWER_ON) {
                power_miliwatts = (long)current_ma[outlet]*(outlet == OUTLET_5V ? 5L : 12L);
                price_per_s += (long)power_miliwatts * PRICE_PER_WATT_SECOND_IN_MILIRUPEES/1000;
            }
        }
        long now = millis();
        funny_assert(now >= last_money_update, F("Back in time"));
        long total_due = price_per_s * (now - last_money_update)/1000;
        if(!money_manager.subtract_money(total_due)) {
            overdue = true;
            overdue_amt = total_due;
            power_off();
        }
        last_money_update = now;
    }
};

Power power;

#define SAME_KEY_MIN_DELAY 500

class KeypadManager {
    uint8_t last_key;
    long last_key_press;
  public:
    void on_mode_set(int m) {
    }

    KeypadManager() : last_key(0),
                      last_key_press(0) {
    }

    void step() {
        uint8_t key = keypad->get_key();
        if (key) {
            if (key == last_key && 
                    last_key_press != 0 &&
                    (millis()-last_key_press) < SAME_KEY_MIN_DELAY) {
                // too early.
                return;

            }
            switch (mode) {
                case MODE_NOTIFICATIONS:
                    if (1<=key && key<=2) {
                        switch (key) {
                            case 1:
                              set_mode(MODE_KEYPAD_INPUT);
                              break;
                            case 2:
                              keypad->calibrate(EEPROM_KEYBOARD_CONFIG_ADDRESS, lcd);
                              break;
                            default:
                              funny_assert(false, F("WTF1"));
                        }
                    } else {
                        lcd_manager.butter(F("Wrong key."), 1000);
                    }
                    break;
                case MODE_KEYPAD_INPUT:
                  if((1<=key && key <= 9) || key == 11) {
                      ui_manager.keypad_display(keypad->char_for_key(key));
                  } else if (key==10) {  // *
                      lcd_manager.butter(F("cancelled..."), 500);
                      set_mode(MODE_NOTIFICATIONS);
                  } else if (key==12) {  // #
                      int len;
                      char* key;
                      ui_manager.get_entered_key(&key, &len);
                      if (key[len-1] == 55 &&
                          key[len-2] == 55) {
                           money_manager.add_money(50000); // 50 rupee
                           lcd_manager.butter(F("50 INR added."), 2000);
                      } else {
                        lcd_manager.butter(F("Wrong code!"), 1000);
                      }
                      set_mode(MODE_NOTIFICATIONS);
                  }
                  break;
                default:
                  // do nothing
                  break;
            }
            last_key_press = millis();
            last_key = key;
        }
    }
};

KeypadManager keypad_manager;


void set_mode(int m) {
    mode = m;
    ui_manager.on_mode_set(m);
    keypad_manager.on_mode_set(m);
}

void client_loop() {
    //ui->display(F("X"));
    //delay(200);
    //return;
    // not reachable
    power.sense();
    power.actions_on_lcd();
    power.update_money();
    lcd_manager.step();
    ui_manager.update_message();
    keypad_manager.step();
   
    //delay(500);
    //lcd.clear();
    //lcd.setCursor(0,0);
    //lcd.print("I'm alive!");
    //delay(1000);


    //uint8_t bytes_read = Serial.readBytes((char*)buf, 1);
    //keypad_step();
    //handle_power_events();
    //ui->step();
}

/*
void handle_power_events() {
    while(scheduler.available()) {
        PowerEvent e = scheduler.pop();
        switch (e.type) {
            case PowerEvent::P12V_ON:
              ui->display(F("12V ON"));
              digitalWrite(GRID_OUTPUT_12V, LOW);
              break;
            case PowerEvent::P12V_OFF:
              ui->display(F("12V OFF"));
              digitalWrite(GRID_OUTPUT_12V, HIGH);
              break;
            case PowerEvent::P5V_ON:
              ui->display(F("5V ON"));
              digitalWrite(GRID_OUTPUT_5V, HIGH);
              break;
            case PowerEvent::P5V_OFF:
              ui->display(F("5V OFF"));
              digitalWrite(GRID_OUTPUT_5V, LOW);
              break;
            default:
              break;
        }
    }
}


void client_instructions() {
    //ui->display(F("Commands:"));
    //ui->display(F("Debug Queue(1)"));
    //ui->display(F("Power Request(2)"));
}*/

void loop() {
    /*
    const int16_t on_stack[] = { 0, 1001, 505, 333, 191, 160, 138, 104, 94, 86, 72, 67, 62 };
    for(int i=0; i<12; ++i) {
        GridUtils::EEPROM_write(EEPROM_KEYBOARD_CONFIG_ADDRESS+2*i, on_stack[i+1]);
    }
    lcd.clear();
    lcd.home();
    lcd.print("done!");
    delay(100000);
    */

    /*
    uint16_t reading = analogRead(A1);
    lcd.clear();
    lcd.home();
    lcd.print(reading);
    delay(200);
    return;*/
    //Serial.println(reading);
    //delay(200);
    //return;
    if (my_number == GENERATOR_NUMBER) {
        generator_loop();
    } else {
        client_loop();
    }
}
