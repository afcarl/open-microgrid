/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:

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


typedef enum {TOKEN_A=1, TOKEN_B=2, TOKEN_C=3, TOKEN_D=4, TOKEN_E=5, TOKEN_E2=6 } state_e;
int state_values [] = {0,0,0,0,0};

int minimal_value = 0;


bool initialization_sequence() {
    long start = millis();
    state_e state = TOKEN_E;
    state_values[0] = minimal_value;
    int cur = smooth_read(10);
    int peak = cur;

    bool success = false;
    while(millis() <= start + 3000 && !success) {
        cur = smooth_read(30) ;
        /*Serial.print("Reading = ");
        Serial.println(cur);
        Serial.print("Peak = ");
        Serial.println(peak);
        Serial.print("State = ");
        Serial.println(state);*/

        switch ((state)) {
            case TOKEN_E:
              peak = max(peak, cur);
              if (cur + 10 < peak) {
                state_values[4] = peak;
                peak = cur;
                state = TOKEN_B;
              }
              break;
            case TOKEN_B:
              peak = min(peak, cur);
              if (cur - 10 > peak) {
                state_values[1] = peak;
                peak = cur;
                state = TOKEN_D;
              }
              break;
            case TOKEN_D:
              peak = max(peak, cur);  
              if (cur + 10 < peak) {
                state_values[3] = peak;
                peak = cur;
                state = TOKEN_C;
              }
              break;
            case TOKEN_C:
              peak = min(peak, cur);
              if (cur - 10 > peak) {
                state_values[2] = peak;
                peak = cur;
                state = TOKEN_E2;
              }
              break;
            case TOKEN_E2:
              if (abs(cur - state_values[4]) < 20) {  // close to E
                success = true;
              }
              break;
            default:
              // do something
              break;
        }
    }
    for (int i=1; i<5; ++i) {
        if (state_values[i-1] +10 >= state_values[i]) {
            success = false;
            break;
        }
    }

    Serial.print("Initialization sequence resulted in ");
    Serial.println(success ? "success" : "failure");
    Serial.print("state = ");
    Serial.println(state);
    for (int i = 0; i<5; ++i) {
        Serial.print("value[");
        Serial.print(i);
        Serial.print("] = ");
        Serial.println(state_values[i]);
    }
    return success;
}


void loop() {

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
        }
    }


    int cur = smooth_read(50);
    Serial.print("Reading = ");
    Serial.println(cur);
/*
    if (cur > minimal_value + 70 && minimal_value +200 < upper && cur + 80 < upper) {
        Serial.println("First black");
        if (initialization_sequence()) {
            Serial.println("Scanning continues");
            long start = millis();
            int oldest=0, old=0, cur = 0;
            int remaining = 17;
            int last_token = 4; 
            while (remaining >=0 && millis() <= start + 10000) {
                cur = smooth_read(2);
                for (int i=0; i<4; ++i) {
                    if (i == last_token) continue;
                    if (abs(oldest - state_values[i]) < 30 &&
                        abs(old - state_values[i]) < 20 &&
                        abs(cur - state_values[i]) < 30) {
                        Serial.print("Got token: ");
                        Serial.println(i);
                        remaining--; 
                        last_token = i;
                        break;
                    }
                }
                oldest = old;
                old = cur;
            }
            Serial.println("Scanning done.");
        } else {
            cur = upper;
            minimal_value = upper;
        }
    }

*/

    minimal_value = min(cur,minimal_value);
}
