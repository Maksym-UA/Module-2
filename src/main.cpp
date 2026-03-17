#include <Arduino.h>

struct Button {
  static const uint8_t ButtonPin = 16;
  volatile bool ButtonState;
  volatile uint32_t NumberButtonPresses;
  const  uint8_t DebounceDelay ; // milliseconds
};

hw_timer_t * MyTimer = NULL;

Button button1 = {false, 0, 50};

unsigned long lastPrintTime = 0;

void IRAM_ATTR onTimer() {
  // Handle button press logic here
  button1.NumberButtonPresses++;
  button1.ButtonState = true;
}

void IRAM_ATTR handleButtonInterrupt() {
    // Restart the timer every time noise is detected
    // This "pushes" the timeout further until the bouncing stops
    timerWrite(MyTimer, 0);
    timerAlarmEnable(MyTimer);
}


void setup() {
  Serial.begin(115200);
  pinMode(Button::ButtonPin, INPUT_PULLUP);

  MyTimer = timerBegin(0, 80, true); // Timer 0, prescaler 80 (1 tick = 1 microsecond), count up

  // Attach the onTimer function
  timerAttachInterrupt(MyTimer, &onTimer, true);

  // Set alarm for 50,000 microseconds (50ms), autoreload = false
  timerAlarmWrite(MyTimer, button1.DebounceDelay * 1000, false);

  // GPIO Interrupt: Trigger on falling edge
  attachInterrupt(digitalPinToInterrupt(Button::ButtonPin), handleButtonInterrupt, FALLING);
}


void loop() {
    if (button1.ButtonState) {
        Serial.print("Button pressed! Total presses: ");
        Serial.println(button1.NumberButtonPresses);
        button1.ButtonState = false; // Reset the button state
    }
}

