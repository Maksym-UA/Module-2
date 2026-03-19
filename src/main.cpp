#include <Arduino.h>

//Configure parameters
static constexpr uint8_t RelayPin = 17;
static constexpr uint8_t SensorPin = 16;
static constexpr uint32_t BaudRate = 115200;

static constexpr uint8_t TotalIterations = 15;

volatile unsigned long StartTime = 0;
volatile unsigned long EndTime = 0;
volatile bool IsMeasuring = false;

//Configure interrupt service routine
void IRAM_ATTR SensorISR() {
  if (EndTime == 0) {
    EndTime = micros();
    IsMeasuring = true;
  }
}

void setup() {
  Serial.begin(BaudRate);
  pinMode(RelayPin, OUTPUT);
  pinMode(SensorPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SensorPin), SensorISR, FALLING);

  Serial.println("Getting ready...");
  delay(2000); // Wait for 2 seconds before starting the first measurement
}

void loop(){

  static unsigned long TotalDuration = 0;
  static uint8_t count = 0;

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
    float average = (TotalDuration / (float)TotalIterations) / 1000.0;
    Serial.print("Average Duration: ");
    Serial.print(average);
    Serial.println(" ms");
    Serial.println("\n--- Restarting measurements ---\n");

    // Reset for next cycle
    count = 0;
    TotalDuration = 0;
    delay(1000); // Wait before starting next cycle
  }
}