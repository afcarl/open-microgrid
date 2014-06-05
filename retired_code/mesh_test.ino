#include <EEPROM.h>
#include <Logging.h>
#include <smart_assert.h>

#define RH_MESH_MAX_MESSAGE_LEN 50
#include <RHMesh.h>
#include <RH_NRF24.h>
#include <SPI.h>


// Singleton instance of the radio driver
RH_NRF24 driver(8, 10);
// This code manages mesh network state and actions - maintains packets,
// discovers routes etc. Address will be later updated 
RHMesh manager(driver, 0);

uint8_t data[] = "Hello from X!";
uint8_t buf[RH_MESH_MAX_MESSAGE_LEN];

uint8_t my_number = 0;

#define NUMBER_OF_NODES 3

// was data exchanged with node i
bool data_exchanged[NUMBER_OF_NODES];
const unsigned long RECEIVE_MODE_DURATION = 3000;

uint8_t len, from;

void setup() 
{
    Log.Init(LOG_LEVEL_DEBUG, 57600);
    while (!Serial) {
      // Wait for serial connect. Only needed for the Leonardo
      delay(20);
    }
    my_number = EEPROM.read(0);
    manager.setThisAddress(my_number);
    data[11] = '0' + my_number;
    for (uint8_t i=0; i<NUMBER_OF_NODES; ++i) {
        data_exchanged[i] = false;
    }
    data_exchanged[my_number] = true;

    randomSeed(analogRead(0));
    if (!manager.init())
        Log.Error("Mesh init failed."CR);
}

void loop() {
    Log.Debug("Loop step for %d."CR, my_number);
    // choosing role randomly
    bool job_finished = true;
    for (uint8_t i = 0; i<NUMBER_OF_NODES; ++i)
        if (!data_exchanged[i])
            job_finished = false;
    if (job_finished) {
      Log.Debug("  Has communicated with all nodes."CR);
    }
    if(!job_finished && random(0, 2) == 0) {
        uint8_t target = 255;
        for (uint8_t i = 0; i<NUMBER_OF_NODES; ++i) {
            if (!data_exchanged[i]) {
                target = i;
                break;
            }
        }
        assert(target != 255);
        Log.Debug("  Trying to communicate with %d"CR, target);
        if (manager.sendtoWait(data, sizeof(data), target) == RH_ROUTER_ERROR_NONE) {
            Log.Debug("    Data transmitted to next hop, waiting for reply"CR);
            // It has been reliably delivered to the next node.
            // Now wait for a reply from the ultimate server
            len = sizeof(buf);
            if (manager.recvfromAckTimeout(buf, &len, 1000, &from))
            {
                Log.Debug("    REPLY from %d: %s"CR, from, (char*)buf);
                if (from == target) {
                    data_exchanged[target] = true;
                    Log.Debug("    Data exchanged successfully with %d."CR, target);
                } else {
                    Log.Debug("    Failure expected reply from %d, but got from %d."CR, target, from);
                }
            }
            else
            {
                Log.Debug("    No reply received from %d."CR, target);
            }
        }
        else {
            Log.Debug("    Send to %d failed."CR, target);
        }
    } else {
        Log.Debug("  Entering receive mode."CR);
        unsigned long when_to_finish = millis() + RECEIVE_MODE_DURATION;
        while (millis() < when_to_finish) {
            unsigned long timeout = when_to_finish - millis();
            if (timeout <= 0)
                break;
            if (manager.recvfromAckTimeout(buf, &len, timeout, &from)) {
                Log.Debug("    REQUEST from %d: %s"CR, from, (char*)buf);
                // Send a reply back to the originator client
                if (manager.sendtoWait(data, sizeof(data), from) == RH_ROUTER_ERROR_NONE) {
                    Log.Debug("    Reply sent!"CR);
                } else {
                    Log.Debug("    Error sending reply!"CR);
                }
            }
        }
    }
}
