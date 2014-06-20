#include "grid_networking.h"

#define RECV_TIMEOUT 1000

GridMessage* GridNetworking::receive_message() {
    len = max_msg_size;
    if (manager.recvfromAckTimeout(buf, &len, RECV_TIMEOUT, &from)) {
        Log.Debug("NETWORK: message from %d (len: %d)"CR, from, len);

        GridMessage* message = GridMessage::parse(buf, len);
        message->from = from;

        return message;
    } else {
        return NULL;
    }
    
}

bool GridNetworking::transmit_message(const GridMessage& request, uint8_t target) {

    Log.Debug("  Trying to communicate with %d"CR, target);

    request.to_buffer(buf, &len);
    if (manager.sendtoWait(buf, len, target) == RH_ROUTER_ERROR_NONE) {
        Log.Debug("NETWORK: Data transmitted to next hop"CR);
        return true;
    }
    else {
        Log.Debug("NETWORK: Send to %d failed."CR, target);
        return false;
    } 
}