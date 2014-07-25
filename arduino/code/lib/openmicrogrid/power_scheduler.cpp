#include "power_scheduler.h"

    
bool PowerScheduler::schedule(const PowerRequestMessage& msg, uint8_t* when) {
    *when = random(msg.start_lb, msg.start_ub + 1);
    return true;
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
    PowerEvent e = events.pop_front();
    return e;
}

void PowerScheduler::print_queue(AbstractUi* ui) {
    ui->display(F("Current time: %D"), millis());
    ui->display(F("Number of elements: %d"), events.size());
    ui->display(F("Events available: %d"), available());
    ui->display(F("Queue contents: "));
    for (int i=0; i<events.size(); ++i) {
        events.get(i).print_me(ui);
    }
    ui->display(F("End of queue debug."));
}
