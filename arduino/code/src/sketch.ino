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
#include "async_lcd_ui.h"
#include "abstract_ui.h"
#include "serial_ui.h"
#include "one_wire_keypad.h"

// Singleton instance of the radio driver
RH_NRF24 driver(8, 10);
// This code manages mesh network state and actions - maintains packets,
// discovers routes etc. Address will be later updated 
RHMesh manager(driver, 0);

AbstractUi* ui;

uint8_t buf[RH_MESH_MAX_MESSAGE_LEN];

GridNetworking network(manager, buf, RH_MESH_MAX_MESSAGE_LEN);

PowerScheduler scheduler;

uint8_t my_number = 0;

OneWireKeypad* keypad;


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
    digitalWrite(GRID_OUTPUT_12V, HIGH);
    digitalWrite(GRID_OUTPUT_5V, HIGH);

    my_number = EEPROM.read(0);
    manager.setThisAddress(my_number);

    assert (manager.init());
    assert (driver.setRF(RH_NRF24::DataRate250kbps, RH_NRF24::TransmitPower0dBm));

    if (my_number == GENERATOR_NUMBER) {
        ui = new SerialUi();
        ui->setup();
        ui->display(F("Generator mode")); // "Generator Mode."
    } else {
        // TODO: change this to LcdUi when ready.
        //ui = new SerialUi();
        ui = new AsyncLcdUi();
        ui->setup();
        
        ui->display(F("Client mode")); // "Client mode."
        client_instructions();
    }
    keypad = new OneWireKeypad();
}


void generator_loop() {
    // currently timeout is 1000ms for receive.
    GridMessage* message = network.receive_message();
    handle_message(message);
}

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

void client_loop() {
    //uint8_t bytes_read = Serial.readBytes((char*)buf, 1);
    keypad_step();
    handle_power_events();
    ui->step();
}

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
              digitalWrite(GRID_OUTPUT_5V, LOW);
              break;
            case PowerEvent::P5V_OFF:
              ui->display(F("5V OFF"));
              digitalWrite(GRID_OUTPUT_5V, HIGH);
              break;
            default:
              break;
        }
    }
}


void client_instructions() {
    ui->display(F("Commands:"));
    ui->display(F("Debug Queue(1)"));
    ui->display(F("Power Request(2)"));
}

void loop() {
    //uint16_t reading = analogRead(A1);
    //Serial.println(reading);
    //delay(200);
    //return;
    if (my_number == GENERATOR_NUMBER) {
        generator_loop();
    } else {
        client_loop();
    }
}
