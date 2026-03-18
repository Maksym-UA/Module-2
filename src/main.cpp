#include <Arduino.h>

struct Button {
  static const uint8_t ButtonPin = 16;

  volatile bool ButtonState;
  volatile uint32_t NumberButtonPresses;

  const  uint16_t DebounceDelay; //ms
  const uint16_t PollingInterval;
};

bool IsButtonPressed = false; // Lock to prevent repeat triggers
bool LastButtonState = HIGH;      // The previous raw reading from the pin
bool StableButtonState = HIGH;    // The confirmed/debounced state
bool PressCounted = false;        // Ensure one count per physical press

unsigned long LastPressTime = 0;  // Last time a press was counted
const unsigned long PressLockout = 200; // ms to ignore subsequent presses after one is counted

unsigned long LastSampleTime = 0;
unsigned long StateStableTime = 0;

Button button = {false, 0, 50, 10};


void setup() {
  Serial.begin(115200);
  pinMode(Button::ButtonPin, INPUT_PULLUP);
}

void loop() {
  unsigned long CurrentTime = millis();

  if (CurrentTime - LastSampleTime >= button.PollingInterval) {
    LastSampleTime = CurrentTime;

    bool RawState = digitalRead(Button::ButtonPin);

    if (RawState != LastButtonState) {
      StateStableTime = CurrentTime; // Reset debounce timer
      LastButtonState = RawState;
    }

    if (CurrentTime - StateStableTime >= button.DebounceDelay) {
      // If the state has finally changed and stayed stable
      if (RawState != StableButtonState) {
        StableButtonState = RawState;

        // Account press only and ignore button release
        if (StableButtonState == LOW && !PressCounted && (CurrentTime - LastPressTime > PressLockout)) {
          Serial.println("Action: Button Pressed (Stable for 50ms)");
          button.NumberButtonPresses++;
          Serial.print("Total Presses: ");
          Serial.println(button.NumberButtonPresses);
          PressCounted = true;
          LastPressTime = CurrentTime;
        }

        // Reset the counted-press lock when the button is released
        if (StableButtonState == HIGH) {
          PressCounted = false;
        }
      }
    }
  }
}
