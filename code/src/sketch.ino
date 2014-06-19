#include <EEPROM.h>
#include <Logging.h>
#include <smart_assert.h>
#include <power_client_api.h>
#include <grid_utils.h>

#define RH_MESH_MAX_MESSAGE_LEN 50
#include <RHMesh.h>
#include <RH_NRF24.h>
#include <SPI.h>


// Singleton instance of the radio driver
RH_NRF24 driver(8, 10);
// This code manages mesh network state and actions - maintains packets,
// discovers routes etc. Address will be later updated 
RHMesh manager(driver, 0);

uint8_t buf[RH_MESH_MAX_MESSAGE_LEN];

uint8_t my_number = 0;


// currently Arduino connected to rpi acts as a "generator". It's number is 1. Other nodes are
// 0 and 2.
#define GENERATOR_NUMBER 1

// control pins for electricity output. Enabled on LOW.
#define GRID_OUTPUT_12V 12
#define GRID_OUTPUT_5V 5


uint8_t len, from;

void setup() 
{
    Log.Init(LOG_LEVEL_DEBUG, 57600);
    while (!Serial) {
      // Wait for serial connect. Only needed for the Leonardo
      delay(20);
    }
    Serial.setTimeout(100);

    pinMode(GRID_OUTPUT_12V, OUTPUT);
    pinMode(GRID_OUTPUT_5V, OUTPUT);

    // off on high
    digitalWrite(GRID_OUTPUT_12V, HIGH);
    digitalWrite(GRID_OUTPUT_5V, HIGH);

    my_number = EEPROM.read(0);
    manager.setThisAddress(my_number);

    assert (manager.init());

    Log.Debug("Setup complete for %d."CR, my_number);
    if (my_number == GENERATOR_NUMBER) {
        Log.Debug("Generator mode."CR);
    } else {
        Log.Debug("Client mode."CR);
        Log.Debug("Press R to issue a power request."CR);
    }
}

void loop() {
    if (my_number == GENERATOR_NUMBER) {
        generator_loop();
    } else {
        client_loop();
    }
}

void generator_loop() {
    delay(1000);
}

void on_power_request() {
    Serial.println(F("Request for power."));
    PowerRequest res = PowerClientApi::power_request_from_stdin();
    Serial.println(F("You created the following power request:"));
    PowerClientApi::power_request_to_stdin(res);
    int temp = GridUtils::get_int(
        F("Do you want to submit to generating node (option 0) or cancel (option 1) [0/1]: "),
        GridUtils::validate01);
    
    if (temp == 0) {
        Serial.println(F("Yeee haw"));
    } else {
        Serial.println(F("buuuu...."));
    }
}

void client_loop() {
    uint8_t bytes_read = Serial.readBytes((char*)buf, 1);
    if (bytes_read > 0) {
        assert(bytes_read == 1);
        switch (buf[0]) {
          case 'R':
            on_power_request();
            break;
          default:
            Serial.println(F("Invalid command."));
        }
    }
}
