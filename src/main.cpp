#include <Arduino.h>
#include <atomic>

struct Button {
  //static тут зайве. З однією кнопкою працювати буде, але якщо їх буде декілька, то у всіх їх ButtonPin буде 16.
  const uint8_t BUTTON_PIN = 16;

  volatile bool ButtonState;
  std::atomic<uint32_t> NumberButtonPresses;
  const uint8_t DEBOUNCE_DELAY; // milliseconds

  Button(bool state, uint32_t presses, uint8_t debounce)
    : ButtonState(state), NumberButtonPresses(presses), DEBOUNCE_DELAY(debounce) {}
};

hw_timer_t * MyTimer = NULL;

Button button1(false, 0, 50);


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
  pinMode(button1.BUTTON_PIN, INPUT_PULLUP);

  MyTimer = timerBegin(0, 80, true); // Timer 0, prescaler 80 (1 tick = 1 microsecond), count up

  // Attach the onTimer function
  timerAttachInterrupt(MyTimer, &onTimer, true);

  // Set alarm for 50,000 microseconds (50ms), autoreload = false
  timerAlarmWrite(MyTimer, button1.DEBOUNCE_DELAY * 1000, false);

  // GPIO Interrupt: Trigger on falling edge
  attachInterrupt(digitalPinToInterrupt(button1.BUTTON_PIN), handleButtonInterrupt, FALLING);
}


void loop() {
    if (button1.ButtonState) {
        Serial.print("Button pressed! Total presses: ");
        Serial.println(button1.NumberButtonPresses);
        button1.ButtonState = false; // Reset the button state
    }
}

