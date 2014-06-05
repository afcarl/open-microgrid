#include <EEPROM.h>
#include <Logging.h>
#include <smart_assert.h>

#define RH_MESH_MAX_MESSAGE_LEN 50
#include <RHMesh.h>
#include <RH_NRF24.h>
#include <SPI.h>


void setup() 
{
    Log.Init(LOG_LEVEL_DEBUG, 57600);
    while (!Serial) {
      // Wait for serial connect. Only needed for the Leonardo
      delay(20);
    }

    
}

bool written = false;

void loop() {
    if (!written) {
        int my_number = EEPROM.read(0);
        Log.Debug("Read %d"CR, my_number);
        EEPROM.write(0, 2);
        Log.Debug("Written %d"CR, my_number-1);
        written = true;
    } else {
        Log.Debug("Already written - current value: %d"CR, EEPROM.read(0));
    }
    delay(1000);
}
