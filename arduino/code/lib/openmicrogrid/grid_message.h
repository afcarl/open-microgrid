#ifndef GRID_MESSAGE_H
#define GRID_MESSAGE_H

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#include "smart_assert.h"
#include "Logging.h"
#include "abstract_ui.h"


class GridMessage {
  public:
    typedef enum {
        POWER_REQUEST_MESSAGE = 1,
        POWER_RESPONSE_MESSAGE
    } message_type;
  public:
    message_type type;
    uint8_t from;

    GridMessage(message_type _type): type(_type) {}

    virtual void describe(AbstractUi*) const;

    virtual void to_buffer(uint8_t* buffer, uint8_t* len) const;

    static GridMessage* parse(const uint8_t* buffer, const uint8_t& len);
};


struct PowerRequestMessage : GridMessage {
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

    PowerRequestMessage() : GridMessage(POWER_REQUEST_MESSAGE) {
    }

    PowerRequestMessage(uint8_t _flags,
                        uint8_t _start_lb,
                        uint8_t _start_ub,
                        uint8_t _duration) : GridMessage(POWER_REQUEST_MESSAGE),
                                             flags(_flags),
                                             start_lb(_start_lb),
                                             start_ub(_start_ub),
                                             duration(_duration) {
    }

    void describe(AbstractUi*) const;
    void to_buffer(uint8_t* buffer, uint8_t* len) const;

    static PowerRequestMessage* parse(const uint8_t* buffer, const uint8_t& len);
};


struct PowerResponseMessage : GridMessage {
    typedef enum {
        GRANTED = 1,
        DENIED
    } response_type;
    // flags from least significant bit:
    // - critical/uncritical (1 for uncritical)
    // - 5V/12V (1 for 12V)
    response_type response;
    uint8_t when;

    PowerResponseMessage() : GridMessage(POWER_RESPONSE_MESSAGE) {
    }

    PowerResponseMessage(response_type _response,
                        uint8_t _when) : GridMessage(POWER_RESPONSE_MESSAGE),
                                         response(_response),
                                         when(_when) {
    }

    void describe(AbstractUi*) const;
    void to_buffer(uint8_t* buffer, uint8_t* len) const;

    static PowerResponseMessage* parse(const uint8_t* buffer, const uint8_t& len);
};


#endif
