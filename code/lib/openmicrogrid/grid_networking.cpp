#include "grid_networking.h"

#define RECV_TIMEOUT 1000

GridMessage* GridNetworking::receive_message() {
    Log.Debug("Generator server action"CR);
    len = max_msg_size;
    if (manager.recvfromAckTimeout(buf, &len, RECV_TIMEOUT, &from)) {
        Log.Debug("    REQUEST from %d (len: %d)"CR, from, len);

        GridMessage* message = GridMessage::parse(buf, len);

        // Send a reply back to the originator client
        uint8_t data [] = "leave me alone!";
        if (manager.sendtoWait(data, sizeof(data), from) == RH_ROUTER_ERROR_NONE) {
            Log.Debug("    Reply sent!"CR);
        } else {
            Log.Debug("    Error sending reply!"CR);
        }
        return message;
    }
    
}

bool GridNetworking::transmit_power_request(const PowerRequestMessage& request, uint8_t target) {

    Log.Debug("  Trying to communicate with %d"CR, target);

    request.to_buffer(buf, &len);
    if (manager.sendtoWait(buf, len, target) == RH_ROUTER_ERROR_NONE) {
        Log.Debug("    Data transmitted to next hop, waiting for reply"CR);
        // It has been reliably delivered to the next node.
        // Now wait for a reply from the ultimate server
        len = max_msg_size;
        if (manager.recvfromAckTimeout(buf, &len, 1000, &from))
        {
            Log.Debug("    REPLY from %d (len: %d): %s"CR, from, len, (char*)buf);
            if (from == target) {
                Log.Debug("    Data exchanged successfully with %d."CR, target);
                return true;
            } else {
                Log.Debug("    Failure expected reply from %d, but got from %d."CR, target, from);
                return false;
            }
        }
        else
        {
            Log.Debug("    No reply received from %d."CR, target);
            return false;
        }
    }
    else {
        Log.Debug("    Send to %d failed."CR, target);
        return false;
    } 
}