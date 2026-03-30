#include <Arduino.h>

// Led state
enum class LedState : uint8_t {
  Off = LOW,
  On = HIGH
};

// Configure parameters
struct Config {
  static constexpr uint8_t Led1 = 16;
  static constexpr uint8_t Led2 = 17;
  static constexpr uint8_t Led3 = 18;
  static constexpr uint32_t BaudRate = 115200;
};

static int TotalBlinks = 99; // Total blinks across all LEDs before stopping


// Class to control an LED independently
class Led {
  private:
    const uint8_t pin;
    unsigned long lastToggleMillis;
    LedState state;
    uint16_t duration;

  public:
    explicit Led(uint8_t pinNumber, uint16_t blinkDuration) :
      pin(pinNumber), lastToggleMillis(0), state(LedState::Off), duration(blinkDuration) {}

    void init() const {
      pinMode(pin, OUTPUT);
      digitalWrite(pin, static_cast<uint8_t>(state));
    }

    // Update returns true if the LED toggled state on this call
    bool update(unsigned long currentMillis) {
      if (currentMillis - lastToggleMillis >= duration) {
        lastToggleMillis = currentMillis;
        state = (state == LedState::Off) ? LedState::On : LedState::Off;
        digitalWrite(pin, static_cast<uint8_t>(state));
        return true;
      }
      return false;
    }
};

static Led Led1(Config::Led1, 200);
static Led Led2(Config::Led2, 500);
static Led Led3(Config::Led3, 1000);

void setup() {
  Serial.begin(Config::BaudRate);
  Led1.init();
  Led2.init();
  Led3.init();

  Serial.println("Getting ready...");
  delay(2000); // Wait for 2 seconds before starting
}

void loop() {
  static int count = 0; // Count total toggles across all LEDs - reviewed by mentor

  if (count >= TotalBlinks) {
    Serial.println("Done blinking!");
    digitalWrite(Config::Led1, static_cast<uint8_t>(LedState::Off));
    digitalWrite(Config::Led2, static_cast<uint8_t>(LedState::Off));
    digitalWrite(Config::Led3, static_cast<uint8_t>(LedState::Off));
    return;
  }

  unsigned long currentMillis = millis();
  bool toggled = false;

  toggled |= Led1.update(currentMillis);
  toggled |= Led2.update(currentMillis);
  toggled |= Led3.update(currentMillis);

  if (toggled) {
    count++;
  }
}
