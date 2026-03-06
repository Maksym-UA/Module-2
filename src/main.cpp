#include <Arduino.h>

//Led state
enum class LedState : uint8_t {
  Off = LOW,
  On = HIGH
};

// Configuration parameters
struct Config {
  static constexpr uint8_t ButtonPin  = 17;
  static constexpr uint8_t LedPin  = 16;
  static constexpr uint16_t BlinkTime = 300;
  static constexpr uint8_t  BlinkCount = 7;
  static constexpr uint32_t PauseInterval = 2000;
  static constexpr uint32_t BaudRate = 115200;
};

// Class to control the LED
class Led {
  private:
    const uint8_t pin;
  public:
    explicit constexpr Led(uint8_t pinNumber) : pin(pinNumber) {}

    void init() const {
      pinMode(pin, OUTPUT);
    }

    void set(LedState state) const {
      digitalWrite(pin, static_cast<uint8_t>(state));
    }
};

void setup() {
  //Serial.begin(115200);
  static const Led led(Config::LedPin);
  pinMode(Config::ButtonPin, INPUT_PULLUP);
  Serial.begin(Config::BaudRate);
  led.init();
}

void loop() {
  static uint32_t lastLoopTime = 0;
  const uint32_t startMicros = micros();

  static const Led led(Config::LedPin);

  static uint32_t lastUpdateTime = 0;
  static uint8_t  blinkCounter = 0;
  static LedState currentState = LedState::Off;
  static bool isPaused = false;

  const uint32_t currentTime = millis();
  const uint32_t currentInterval = isPaused ? Config::PauseInterval : Config::BlinkTime;

  if (currentTime - lastUpdateTime >= currentInterval) {
    lastUpdateTime = currentTime;

    if (isPaused) {
      //Pause is over, reset counters and start blinking again
      isPaused = false;
      blinkCounter = 0;
      currentState = LedState::On;
    } else {
      //Toggle LED state
      if (currentState == LedState::On) {
          currentState = LedState::Off;
          blinkCounter++;
      } else {
          currentState = LedState::On;
      }

      // If we've completed the required number of blinks (on + off)
      if (blinkCounter >= Config::BlinkCount) {
        isPaused = true;
        currentState = LedState::Off;
      }
    }
    led.set(currentState);
  }
  const uint32_t endMicros = micros();
  const uint32_t loopDuration = endMicros - startMicros;

  // Log loop execution time every second to avoid flooding the serial output
  static uint32_t lastSerialTime = 0;
    if (millis() - lastSerialTime >= 1000) {
        lastSerialTime = millis();
        Serial.printf("Loop execution time: %lu ms\n", loopDuration);
    }
}