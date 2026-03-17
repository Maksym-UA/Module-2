#include <Arduino.h>

struct Button {
  static const uint8_t ButtonPin = 16;

  volatile bool ButtonState;
  volatile uint32_t NumberButtonPresses;

  const  uint16_t DebounceDelay; // milliseconds

};

bool IsButtonPressed = false; // Lock to prevent repeat triggers

unsigned long LastDebounceTime = 0;  // the last time the output pin was toggled
Button button = {false, 0, 50};


void IRAM_ATTR HandleButtonInterrupt() {
// Handle button press logic here
  button.ButtonState = true;
}

void setup() {
  Serial.begin(115200);
  pinMode(Button::ButtonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(Button::ButtonPin), HandleButtonInterrupt, FALLING);
}

void loop() {
  if (button.ButtonState) {
    unsigned long CurrentTime = millis();

    if ((CurrentTime - LastDebounceTime) > button.DebounceDelay) {

      bool CurrentState = (digitalRead(Button::ButtonPin) == LOW);

      //check if the button is still pressed after debounce delay
      if (CurrentState && !IsButtonPressed) {
        Serial.println("Confirmed: Button was pressed.");

        IsButtonPressed = true;


      } else if (!CurrentState) {
        Serial.println("False trigger: Button was NOT pressed.");
        IsButtonPressed = false;
      }

      Serial.print("Total presses: ");
      Serial.println(button.NumberButtonPresses);

      button.NumberButtonPresses++;
      LastDebounceTime = CurrentTime;
      button.ButtonState = false; // Reset button state after processing
    }
  }
}
