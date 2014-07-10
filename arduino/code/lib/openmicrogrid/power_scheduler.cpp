#include "power_scheduler.h"

    
bool PowerScheduler::schedule(const PowerRequestMessage& msg, uint8_t* when) {
    *when = random(msg.start_lb, msg.start_ub + 1);
    return random(0, 2) == 0;
}


void PowerScheduler::push(PowerEvent e) {
    int i = 0;
    // i is index of first violation. Insert at i inserts before first violation.
    while(i < events.size() && events.get(i).e_time <= e.e_time) ++i;
    events.add(i, e);

}

bool PowerScheduler::available() {
    return events.size() > 0 && events.get(0).e_time <= millis();
}

PowerEvent PowerScheduler::pop() {
    return events.shift();
}