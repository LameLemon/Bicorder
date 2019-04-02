#include <Arduino.h>

class Button {
    private:
        const int debounceDelay = 200;
        int buttonState = 0;
        int pin;
        int lastButtonState = LOW;
        unsigned long lastDebounceTime = 0;
        int ret_state = 0;
  
    public:
        // Constructor: set button pin
        Button(int sw) { 
            pin = sw; 
        }

        // Check if button is pushed or not
        int state(void) {
            if (ret_state) {
                ret_state = 0;
                return 1;
            } else {
                return ret_state;
            }
        }

        // Call on every loop to update button state
        void check(void) {
            int reading = digitalRead(pin);
  
            if (reading != lastButtonState) {
                lastDebounceTime = millis();
            }

            if ((millis() - lastButtonState) > debounceDelay) {
                if (reading != buttonState) {
                    buttonState = reading;

                    if (buttonState == HIGH){
                        ret_state = 1;
                    } 
                } else {
                    ret_state = 0;
                }
            }   

            lastButtonState = reading;
        }
};