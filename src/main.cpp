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
  if (!button.ButtonState) return;

  unsigned long currentTime = millis();
  if ((currentTime - LastDebounceTime) <= button.DebounceDelay) return;

  button.ButtonState = false;
  LastDebounceTime = currentTime;

  if (digitalRead(Button::ButtonPin) == LOW) {
    button.NumberButtonPresses++;
    Serial.print("Total presses: ");
    Serial.println(button.NumberButtonPresses);
  }
}