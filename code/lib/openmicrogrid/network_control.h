#ifndef NETWORK_CONTROL_H
#define NETWORK_CONTROL_H
#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

class ControlMessage {
  public:
    /*! 
     * default Constructor
     */
    ControlMessage() : _message_count(0) {
    };
    
    /**
    * Produce a join request message
    *
    * \param netadr proposed address for node.
    * \return data to send over communications channel.
    */
    String join_request(uint64_t netadr);

};

extern ControlMessage control_message;
#endif




