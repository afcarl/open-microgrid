
bool read_tokens(int deadline, int number);

int led = 9;
int photores = 0;

long start_time = 0;
long read_counter = 0;

bool calibrated = false;

// the setup routine runs once when you press reset:
void setup() {         

  pinMode(led, OUTPUT);     

  Serial.begin(57600);
  while (!Serial) {
    // Wait for serial connect. Only needed for the Leonardo
    delay(20);
  }  
  start_time = millis();
  // initialize the digital pin as an output.
}

int buffer[10];



uint16_t lower, upper;


int smooth_read(int ms){
    long start = millis();
    long value = 0;
    int readings = 0;
    while(readings < 10 || millis() <= start + (long)ms) {
        value += analogRead(photores);
        readings++;
    }
    return value / readings;
}


int state_values[5];

int minimal_value = 0;


bool initialization_sequence() {
    bool success = true;
    state_values[0] = minimal_value;
    // make sure we are peak white.
    int peak = smooth_read(2);
    int cur = peak;
    while (cur + 10 > peak) {
        cur = smooth_read(2);
        peak = max(peak, cur);
    }
    state_values[4] = peak;
    if (!read_tokens(3000, 3))
        return false;
    for (int i=0; i<3; ++i) {
        state_values[i+1] = buffer[i];
    }
    for (int i=1; i<5; ++i) {
        if (state_values[i-1] +10 >= state_values[i]) {
            success = false;
            break;
        }
    }

    Serial.print("Initialization sequence resulted in ");
    Serial.println(success ? "success" : "failure");
    for (int i = 0; i<5; ++i) {
        Serial.print("value[");
        Serial.print(i);
        Serial.print("] = ");
        Serial.println(state_values[i]);
    }
    return success;
}


bool read_tokens(int deadline, int number) {
    long start = millis();
    int cur = 0;
    int remaining = number;
    // 0 - white, 1 - other
    int state = 0;
    int peak = 0;
    int index = 0;
    while (remaining >0 && millis() <= start + deadline) {
        cur = smooth_read(1);
        switch (state) {
            case 0:
                if (cur + 30 < state_values[4]) {
                    state = 1;
                    peak = cur;
                }                      
                break;
            case 1:
                peak = min(cur, peak);
                if (cur +15 >= state_values[4]) {
                    state = 0;
                    // pick the one closest to optimal value
                    buffer[index++] = peak;
                    remaining--; 
                    Serial.print(remaining);
                    Serial.print(" ");
                }
                break;
            default:
                break;
                // do something
        }
    }
    Serial.println();
    return remaining == 0;
}

char text[10];

bool print_code() {
    int sum = 0;
    Serial.print("Analyzing code ");
    for (int i=0; i<9; ++i) {
        Serial.print(buffer[i]);
        Serial.print(" ");
    }
    Serial.println();
    for (int i=0; i<8; ++i) {
        sum += buffer[i];
    }
    if ((sum % 4) != buffer[8]) {
        Serial.println("Parity failed!");
        return false;
    } else {
        int index = 0;
        for(int i=0; i<8; i+=2) {
            char c = 4*buffer[i] + buffer[i+1];
            if (0 <= c && c <= 9) c += '0';
            else c = c - 10 + 'a';
            text[index++] = c;
        }
        text[index++] = 0;
        Serial.print("Got code: ");
        Serial.print(text);
        Serial.println("!!!!!!!!!!!!!!!");
        return true;
    }

}

void loop() {
    /*
    if(!calibrated) {
        digitalWrite(led, HIGH);
    }
    Serial.println(smooth_read(10));
    delay(100);
    return;
    */
    if (!calibrated) {
        lower = smooth_read(500);
        digitalWrite(led, HIGH);
        upper = smooth_read(500);

        if (lower + 100 < upper) {
            Serial.print("Lower, upper = ");
            Serial.print(lower);
            Serial.print(", ");
            Serial.println(upper);
            minimal_value = upper;
            calibrated = true;
        } else {
            Serial.println("Calibration failed");
            return;
        }
    }


    int cur = smooth_read(50);

    if (cur > minimal_value + 150 && minimal_value +200 < upper) {
        Serial.println("First black");
        if (initialization_sequence()) {
            Serial.println("Scanning continues");
            if (read_tokens(10000, 9)) {
                for (int j=0; j<9; ++j) {
                    int token = 0;
                    int peak = buffer[j];
                    for (int i=0; i<4; ++i) {
                        if (abs(peak - state_values[i]) < abs(peak - state_values[token])) {
                            token = i;
                        }
                    }
                    Serial.print("Peak ");
                    Serial.print(peak);
                    Serial.print(" generates ");
                    Serial.println(token);
                    buffer[j] = token;
                }
                print_code();
                Serial.println("Scanning done.");
            } else {
                Serial.println("Scanning failure.");
            }
        } else {
            Serial.println("Init failed");
            cur = upper;
            minimal_value = upper;
        }
    }



    minimal_value = min(cur,minimal_value);
}
