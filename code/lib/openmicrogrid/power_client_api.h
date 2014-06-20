#ifndef POWER_CLIENT_API_H
#define POWER_CLIENT_API_H

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif
#include "grid_utils.h"
#include "grid_message.h"
#include <Logging.h>

class PowerClientApi {
  public:
    PowerClientApi() {};
    
    static PowerRequestMessage power_request_from_stdin();
    static void power_request_to_stdin(const PowerRequestMessage& request);
};


#endif



