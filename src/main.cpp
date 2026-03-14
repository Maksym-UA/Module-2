#include <Arduino.h>

//Led state
enum class LedState : uint8_t {
  Off = LOW,
  On = HIGH
};

//Configure parameters
// Configuration parameters
struct Config {
  static constexpr uint8_t Led1 = 16;
  static constexpr uint8_t Led2 = 17;
  static constexpr uint8_t Led3 = 18;
  static constexpr uint32_t BaudRate = 115200;

  unsigned long storedTimestamp = 0;
};

static int  TotalBlinks = 15;
volatile bool isBlinking = false;

int count = 0;

// Class to control the LED
class Led {
  private:
    const uint8_t pin;
    const uint8_t duration; // Duration in milliseconds for which the LED will be on
    const unsigned long storedTimestamp; // Timestamp to manage blinking intervals
  public:
    explicit constexpr Led(
      uint8_t pinNumber, uint8_t blinkDuration, unsigned long storedTimestamp) :
      pin(pinNumber), duration(blinkDuration), storedTimestamp(storedTimestamp) {}

    void init() const {
      pinMode(pin, OUTPUT);
    }

    void set(LedState state) const {
      digitalWrite(pin, static_cast<uint8_t>(state));
    }

    void setStoredTimestamp(unsigned long timestamp) {
      const_cast<unsigned long&>(storedTimestamp) = timestamp;
    }

    uint8_t getPin() const {
      return pin;
    }

    uint8_t getBlinkDuration() const {
      return duration;
    }
    unsigned long getStoredTimestamp() const {
      return storedTimestamp;
    }
};

static  Led Led1 (Config::Led1, 200, 0);
static  Led Led2 (Config::Led2, 500, 0);
static  Led Led3 (Config::Led3, 1000, 0);

void setup() {
  Serial.begin(Config::BaudRate);
  Led1.init();
  Led2.init();
  Led3.init();

  Serial.println("Getting ready...");
  delay(2000); // Wait for 2 seconds before starting the first measurement
}

void loop(){
  if (count < TotalBlinks) {

    volatile bool isBlinking = false;
    static bool isPaused = false;
    static LedState currentState = LedState::Off;
    unsigned long currentMillis = millis();

    if (!isBlinking && !isPaused) {
      isBlinking = true;
      currentState = (currentState == LedState::Off) ? LedState::On : LedState::Off;

      Led1.set(currentState);
      if (currentMillis - Led1.getStoredTimestamp() >= Led1.getBlinkDuration()) {
        Led1.setStoredTimestamp(currentMillis);
        digitalWrite(Led1.getPin(), static_cast<uint8_t>(currentState));
      }

      Led2.set(currentState);
      if (currentMillis - Led2.getStoredTimestamp() >= Led2.getBlinkDuration()) {
        Led2.setStoredTimestamp(currentMillis);
        digitalWrite(Led2.getPin(), static_cast<uint8_t>(currentState));
      }

      Led3.set(currentState);
      if (currentMillis - Led3.getStoredTimestamp() >= Led3.getBlinkDuration()) {
        Led3.setStoredTimestamp(currentMillis);
        digitalWrite(Led3.getPin(), static_cast<uint8_t>(currentState));
      }

      count++;
    }
    else {
      if (!isPaused) {
        Serial.println("Blinking paused. Press any key to resume...");
        isPaused = true;
      }
      if (Serial.available() > 0) {
        Serial.read(); // Clear the input buffer
        isBlinking = false;
        isPaused = false;
      }
    }
  }
}