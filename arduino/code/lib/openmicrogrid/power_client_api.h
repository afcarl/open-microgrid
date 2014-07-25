#ifndef POWER_CLIENT_API_H
#define POWER_CLIENT_API_H

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif
#include "grid_utils.h"
#include "grid_message.h"
#include "abstract_ui.h"
#include "one_wire_keypad.h"
#include <Logging.h>

class PowerClientApi {
  public:
    PowerClientApi() {};
    
    static PowerRequestMessage power_request_from_stdin(AbstractUi* ui, OneWireKeypad* keypad);
};


#endif



