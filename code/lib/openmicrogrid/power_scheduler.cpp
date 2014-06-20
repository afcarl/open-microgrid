#include "power_scheduler.h"

    
bool PowerScheduler::schedule(const PowerRequestMessage& msg, uint8_t* when) {
    *when = random(msg.start_lb, msg.start_ub + 1);
    return random(0, 2) == 0;
}
