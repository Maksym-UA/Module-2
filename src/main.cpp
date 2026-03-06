#include <Arduino.h>

//Led state
enum class LedState : uint8_t {
  Off = LOW,
  On = HIGH
};

// Клас-конфігурація зі статичними константами
struct Config {
  static constexpr uint8_t ButtonPin  = 17;
  static constexpr uint8_t LedPin  = 16;
  static constexpr uint16_t BlinkTime = 500;
  static constexpr uint8_t  BlinkCount = 5;
  static constexpr uint32_t PauseInterval = 2000;
};

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
    static const Led led(Config::LedPin);
    led.init();
}

void loop() {
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
      // Закінчили велику паузу, починаємо нову серію
      isPaused = false;
      blinkCounter = 0;
      currentState = LedState::On;
    } else {
      // Логіка перемикання всередині серії
      if (currentState == LedState::On) {
          currentState = LedState::Off;
          blinkCounter++;
      } else {
          currentState = LedState::On;
      }

      // Якщо виконали потрібну кількість блимань (on + off)
      if (blinkCounter >= Config::BlinkCount) {
        isPaused = true;
        currentState = LedState::Off;
      }
    }
    led.set(currentState);
  }
}

