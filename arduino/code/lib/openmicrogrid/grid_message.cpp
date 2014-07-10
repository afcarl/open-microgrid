#include "grid_message.h"

GridMessage* GridMessage::parse(const uint8_t* buffer, const uint8_t& len) {
      GridMessage* result;
      if (len <= 0)
          return NULL;
      switch(buffer[0]) {
        case POWER_REQUEST_MESSAGE:  //Power Request Message
          return PowerRequestMessage::parse(buffer, len);
          break;
        case POWER_RESPONSE_MESSAGE:  //Power Request Message
          return PowerResponseMessage::parse(buffer, len);
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
    if (len != 5)
        return NULL;
    PowerRequestMessage* res = new PowerRequestMessage(buffer[1],
                                                       buffer[2],
                                                       buffer[3],
                                                       buffer[4]);
    return res;
}

void PowerRequestMessage::describe() const {
    Serial.print(F("Request for "));
    Serial.print((flags & (1<<0)) ? F("uncritical") : F("critical"));
    Serial.print(F(" load at "));
    Serial.print((flags & (1<<1)) ? F("12V") : F("5V"));
    Serial.print(F(". Starting between "));
    Serial.print(start_lb);
    Serial.print(F(" and "));
    Serial.print(start_ub);
    Serial.print(F(" seconds from now. The power will flow for "));
    Serial.print(duration);
    Serial.println(F(" seconds."));
}


void PowerResponseMessage::to_buffer(uint8_t* buffer, uint8_t* len) const {
    buffer[0] = type;
    buffer[1] = response;
    buffer[2] = when;
    *len = 3;
}

PowerResponseMessage* PowerResponseMessage::parse(const uint8_t* buffer,
                                                  const uint8_t& len) {
    if (len != 3)
        return NULL;

    PowerResponseMessage* res = new PowerResponseMessage((response_type)buffer[1],
                                                         buffer[2]);
    return res;
}

void PowerResponseMessage::describe() const {
    Serial.print(F("Request was "));
    switch(response) {
      case GRANTED:
        Serial.print(" granted. Yeee haw!");
        Serial.print(" Power will flow ");
        Serial.print(when);
        Serial.println(" seconds from now.");
        break;
      case DENIED:
        Serial.println("denied.");
      default:
        break;
    }
}
