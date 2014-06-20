#include "power_client_api.h"


PowerRequestMessage PowerClientApi::power_request_from_stdin() {
    PowerRequestMessage res;

    int temp;
    res.flags = 0;

    temp = GridUtils::get_int(F("Is this a critical (option 0) or uncritical (option 1) load [0/1]: "),
                              GridUtils::validate01);
    res.flags |= temp*(1<<0);

    temp = GridUtils::get_int(F("Is this a 5V (option 0) or 12V (option 1) load [0/1]: "),
           GridUtils::validate01);
    res.flags |= temp*(1<<1);

    Serial.println(F("You will be now asked to enter two numbers (LB and UB)"));
    Serial.println(F("Their meaning is the following. You will start receiving"));
    Serial.println(F("power between LB and UB seconds from now. For example if"));
    Serial.println(F("LB=1 and UB=5 you will start receiving power between 1"));
    Serial.println(F("and 5 seconds from now."));

    while (true) {
        temp = GridUtils::get_int(F("Enter LB: "), GridUtils::validate_byte);
        res.start_lb = temp;
        temp = GridUtils::get_int(F("Enter UB: "), GridUtils::validate_byte);
        res.start_ub = temp;
        if (res.start_lb <= res.start_ub) break;
        else Serial.println(F("LB must be less or equal to UB."));
    }

    temp = GridUtils::get_int(F("For how long do you need power (in seconds): "), GridUtils::validate_byte);
    res.duration = temp;
    return res;

}

void PowerClientApi::power_request_to_stdin(const PowerRequestMessage& request) {
    Serial.print(F("Request for "));
    Serial.print((request.flags & (1<<0)) ? F("uncritical") : F("critical"));
    Serial.print(F(" load at "));
    Serial.print((request.flags & (1<<1)) ? F("12V") : F("5V"));
    Serial.print(F(". Starting between "));
    Serial.print(request.start_lb);
    Serial.print(F(" and "));
    Serial.print(request.start_ub);
    Serial.print(F(" seconds from now. The power will flow for "));
    Serial.print(request.duration);
    Serial.println(F(" seconds."));
}

