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

void PowerRequestMessage::describe(AbstractUi* ui) const {
    ui->display(F("Request for "));
    ui->display((flags & (1<<0)) ? F("uncritical") : F("critical"));
    ui->display(F(" load at "));
    ui->display((flags & (1<<1)) ? F("12V") : F("5V"));
    ui->display(F("Starting between %d and %d"), start_lb, start_ub);
    ui->display(F("seconds from now."));
    ui->display(F(" The power will flow for %d seconds."), duration);
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

void PowerResponseMessage::describe(AbstractUi* ui) const {
    switch(response) {
      case GRANTED:
        ui->display(F("Request granted."));
        ui->display(F("Wait %d seconds."), when);
        break;
      case DENIED:
        Serial.println(F("Request was denied."));
      default:
        break;
    }
}
