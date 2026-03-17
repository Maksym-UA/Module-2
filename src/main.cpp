#include <Arduino.h>

struct Button {
  static const uint8_t ButtonPin = 16;
  volatile bool ButtonState;
  volatile uint32_t NumberButtonPresses;
};

Button button1 = {false, 0};

unsigned long lastPrintTime = 0;

void IRAM_ATTR handleButtonPress() {
  // Handle button press logic here
  button1.NumberButtonPresses++;
  button1.ButtonState = true;
}


void setup() {
  Serial.begin(115200);
  pinMode(Button::ButtonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(Button::ButtonPin), handleButtonPress, FALLING);
}


void loop() {
    unsigned long currentTime = millis();
    if (button1.ButtonState && (currentTime - lastPrintTime >= 50)) {
        Serial.print("Button pressed! Total presses: ");
        Serial.println(button1.NumberButtonPresses);
        button1.ButtonState = false; // Reset the button state
        lastPrintTime = currentTime;
    }
}

