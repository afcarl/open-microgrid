#ifndef POWER_CLIENT_API_H
#define POWER_CLIENT_API_H

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif
#include "grid_utils.h"


struct PowerRequest {
    // flags from least significant bit:
    // - critical/uncritical (1 for uncritical)
    // - 5V/12V (1 for 12V)
    uint8_t flags;
    // time variables:
    // - power must start flowing between start_lb and start_ub
    // - power must flow for at least duration
    uint8_t start_lb;
    uint8_t start_ub;
    uint8_t duration;
};

class PowerClientApi {
  public:
    PowerClientApi() {};
    
    static PowerRequest power_request_from_stdin();
    static void power_request_to_stdin(const PowerRequest& request);
};


#endif



