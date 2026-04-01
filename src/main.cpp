#include <Arduino.h>

enum ButtonFSMState {
  IDLE,
  DEBOUNCING_PRESS,
  PRESSED,
  DEBOUNCING_RELEASE
};

struct Button {
  static const uint8_t BUTTON_PIN = 16;

  volatile uint32_t NumberButtonPresses;

  const uint16_t DEBOUNCE_DELAY; //ms
  const uint16_t POLLING_INTERVAL;

  Button(uint32_t presses, uint16_t debounce, uint16_t polling)
    : NumberButtonPresses(presses),
      DEBOUNCE_DELAY(debounce), POLLING_INTERVAL(polling) {}
};



void setup() {
  Serial.begin(115200);
  pinMode(Button::BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  static ButtonFSMState State = IDLE;
  static unsigned long LastSampleTime = 0;
  static unsigned long StateBaseTime = 0;  // Time when state changed
  static bool LastRawState = HIGH;
  static Button button = {0, 50, 10};

  unsigned long CurrentTime = millis();

  if (CurrentTime - LastSampleTime < button.POLLING_INTERVAL) {
    return;  // Not yet time to sample
  }
  LastSampleTime = CurrentTime;

  bool RawState = digitalRead(Button::BUTTON_PIN);

  switch (State) {
    case IDLE:
      if (RawState == LOW) {
        State = DEBOUNCING_PRESS;
        StateBaseTime = CurrentTime;
      }
      break;

    case DEBOUNCING_PRESS:
      if (RawState == HIGH) {
        // Pin went back HIGH, stay in IDLE
        State = IDLE;
      } else if (CurrentTime - StateBaseTime >= button.DEBOUNCE_DELAY) {
        // Pin stayed LOW long enough
        Serial.println("Action: Button Pressed (Stable for 50ms)");
        button.NumberButtonPresses++;
        Serial.print("Total Presses: ");
        Serial.println(button.NumberButtonPresses);
        State = PRESSED;
      }
      break;

    case PRESSED:
      if (RawState == HIGH) {
        State = DEBOUNCING_RELEASE;
        StateBaseTime = CurrentTime;
      }
      break;

    case DEBOUNCING_RELEASE:
      if (RawState == LOW) {
        // Pin went back LOW, stay in PRESSED
        State = PRESSED;
      } else if (CurrentTime - StateBaseTime >= button.DEBOUNCE_DELAY) {
        // Pin stayed HIGH long enough, button release confirmed
        State = IDLE;
      }
      break;
  }

  LastRawState = RawState;
}
