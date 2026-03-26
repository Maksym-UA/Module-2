
#include <Arduino.h>


//Configuration
//LED status enum
enum class LedStatus {
  OFF,
  ON,
  BLINK
};

// Parameters
struct Config {
  static constexpr uint8_t Led1 = 16;
  static constexpr uint8_t Led2 = 17;
  static constexpr uint8_t Led3 = 18;
  static constexpr uint32_t BaudRate = 115200;
};