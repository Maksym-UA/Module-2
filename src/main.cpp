#include <Arduino.h>
#include <atomic>

struct Button {
  static const uint8_t ButtonPin = 16;
  volatile bool ButtonState;
  std::atomic<uint32_t> NumberButtonPresses{0};// Initialize atomic counter to 0
};

Button button1;



void IRAM_ATTR handleButtonPress() {
  // Handle button press logic here
  button1.NumberButtonPresses.fetch_add(1, std::memory_order_relaxed); //atomic increment
  button1.ButtonState = true;
}


void setup() {
  Serial.begin(115200);
  pinMode(Button::ButtonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(Button::ButtonPin), handleButtonPress, FALLING);
}


void loop() {
    static unsigned long lastPrintTime = 0; // mentor  review
    unsigned long currentTime = millis();

    if (button1.ButtonState && (currentTime - lastPrintTime >= 50)) {
      uint32_t presses = button1.NumberButtonPresses.load(std::memory_order_relaxed);
        Serial.print("Button pressed! Total presses: ");
      Serial.println(presses);
        button1.ButtonState = false; // Reset the button state
        lastPrintTime = currentTime;
    }
}

