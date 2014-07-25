#include "power_client_api.h"


PowerRequestMessage PowerClientApi::power_request_from_stdin(AbstractUi* ui, OneWireKeypad* keypad) {
    PowerRequestMessage res;

    int temp;
    res.flags = 0;
    //ui->display(F("Is this a critical (option 0) or uncritical (option 1) load?"));
    temp = ui->get_binary(F("Uncritical?"), keypad);
    res.flags |= temp*(1<<0);


    //ui->display(F("Is this a 5V (option 0) or 12V (option 1) load?"));
    temp = ui->get_binary(F("5V(0) or 12V(1)?"), keypad);
    res.flags |= temp*(1<<1);

    /*ui->display(F("You will be now asked to enter two numbers (LB and UB)"));
    ui->display(F("Their meaning is the following. You will start receiving"));
    ui->display(F("power between LB and UB seconds from now. For example if"));
    ui->display(F("LB=1 and UB=5 you will start receiving power between 1"));
    ui->display(F("and 5 seconds from now."));*/

    while (true) {
        res.start_lb = ui->get_integer(F("Enter LB:"), keypad);
        res.start_ub = ui->get_integer(F("Enter UB:"), keypad);;
        if (res.start_lb <= res.start_ub) break;
        else ui->display(F("LB must be less or equal to UB."));
    }
    //ui->display(F("For how long do you need power (in seconds): "));
    temp = ui->get_integer(F("Duration?"), keypad);
    res.duration = temp;
    return res;
}

