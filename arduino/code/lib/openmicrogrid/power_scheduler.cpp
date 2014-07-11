#include "power_scheduler.h"

    
bool PowerScheduler::schedule(const PowerRequestMessage& msg, uint8_t* when) {
    *when = random(msg.start_lb, msg.start_ub + 1);
    return true;
}


void PowerScheduler::push(PowerEvent e) {
    Serial.print(F("Adding event to queue: "));
    e.print_me();
    int i = 0;
    // i is index of first violation. Insert at i inserts before first violation.
    while(i < events.size() && events.get(i).e_time <= e.e_time) ++i;
    events.add(i, e);

}

bool PowerScheduler::available() {
    return events.size() > 0 && events.get(0).e_time <= millis();
}

PowerEvent PowerScheduler::pop() {
    PowerEvent e = events.shift();
    Serial.print(F("Removing event from queue: "));
    e.print_me();
    return e;
}

void PowerScheduler::print_queue() {
    Serial.print(F("Current time: "));
    Serial.println(millis());
    Serial.print(F("Number of elements in queue: "));
    Serial.println(events.size());
    Serial.print("Events available: ");
    Serial.println(available());
    Serial.println(F("Queue contents: "));
    for (int i=0; i<events.size(); ++i) {
        events.get(i).print_me();
    }
    Serial.println(F("End of queue debug"));
}
