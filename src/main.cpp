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
};

static int  TotalIterations = 15;
volatile unsigned long StartTime = 0;
volatile unsigned long EndTime = 0;
volatile bool isBlinking = false;

unsigned long TotalDuration = 0;
int count = 0;

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

    uint8_t getPin() const {
      return pin;
    }
};

//Configure interrupt service routine
void IRAM_ATTR LedISR() {
  if (EndTime == 0) {
    EndTime = micros();
    isBlinking = true;
  }
}

static const Led Led1 (Config::Led1);
static const Led Led2 (Config::Led2);
static const Led Led3 (Config::Led3);

void setup() {
  Serial.begin(Config::BaudRate);
  Led1.init();
  Led2.init();
  Led3.init();
  attachInterrupt(digitalPinToInterrupt(Led1.getPin()), LedISR, FALLING);

  Serial.println("Getting ready...");
  delay(2000); // Wait for 2 seconds before starting the first measurement
}

void loop(){
  if (count < TotalIterations) {
    EndTime = 0; // Reset end time for the next measurement
    IsMeasuring = false; // Reset measuring flag

    Serial.print("Start mesurement #");
    Serial.print(count + 1);
    Serial.print(" of ");
    Serial.println(String(TotalIterations) );

    StartTime = micros();
    digitalWrite(RelayPin, HIGH);

    //wait realy to activate or timeout after 1 second
    unsigned long timeout = millis();// + 1000;
    while (!IsMeasuring && (millis() - timeout < 1000)) {

      yield();
    }

    if(IsMeasuring){
      unsigned long duration = EndTime - StartTime;
      TotalDuration += duration;
      Serial.print("Duration: ");
      Serial.print(duration/1000.0); // Convert to milliseconds
      Serial.println(" ms");
      count++;
    } else {
      Serial.println("Measurement timed out!");
    }

    digitalWrite(RelayPin, LOW);
    delay(1000); // Wait for 1 second before the next measurement

  } else if (count == TotalIterations) {
    float average = (TotalDuration / TotalIterations) / 1000.0;
    Serial.print("Average Duration: ");
    Serial.print(average);
    Serial.println(" ms");
    count++;
  }
}