#ifndef GRID_NETWORKING_H
#define GRID_NETWORKING_H

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#include "power_client_api.h"
#include "RHMesh.h"
#include "Logging.h"
#include "grid_message.h"


class GridNetworking {
    RHMesh& manager;
    uint8_t * buf;
    const int max_msg_size;
    uint8_t len;
    uint8_t from;

  public:
    GridNetworking(RHMesh& _manager, uint8_t * _buf, const int _mx) : 
            manager(_manager),
            buf(_buf),
            max_msg_size(_mx) {
    };

    GridMessage* receive_message();
    bool transmit_power_request(const PowerRequestMessage& request, uint8_t target);
};


#endif



