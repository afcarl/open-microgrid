#include "grid_message.h"

GridMessage* GridMessage::parse(const uint8_t* buffer, const uint8_t& len) {
      GridMessage* result;
      if (len <= 0)
          return NULL;

      switch(buffer[0]) {
        case POWER_REQUEST_MESSAGE:  //Power Request Message
          return PowerRequestMessage::parse(buffer, len);
          break;
        default:
          return NULL;
      }
  }


void PowerRequestMessage::to_buffer(uint8_t* buffer, uint8_t* len) const {
    buffer[0] = type;
    buffer[1] = flags;
    buffer[2] = start_lb;
    buffer[3] = start_ub;
    buffer[4] = duration;
    *len = 5;
}

PowerRequestMessage* PowerRequestMessage::parse(const uint8_t* buffer,
                                                       const uint8_t& len) {
    Log.Debug("PowerRequestMessage"CR);
    if (len != 5)
        return NULL;
    PowerRequestMessage* res = new PowerRequestMessage(buffer[1],
                                                       buffer[2],
                                                       buffer[3],
                                                       buffer[4]);
    return res;
}

