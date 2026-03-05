#include <Arduino.h>

#define ADC_PIN 4
#define RELAY_PIN 17
#define VREF 3.0
#define ADC_RESOLUTION 4095.0
#define LED_PIN 16
#define VOLTAGE_THRESHOLD 1.3
#define VOLTAGE_GIST 0.2

volatile bool relayState = false; // Track current state (false = OFF, true = ON)

float getVoltage(int adcValue){
  //convert ADC value to voltage on the sensor
  return ((adcValue / ADC_RESOLUTION) * VREF);
}


void setup() {
  Serial.begin(115200); //start the serial monitor at baude rate of 115200
  pinMode (LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Default OFF
  analogReadResolution(12); //set the ADC resolution to 12 bits (0-4095)
  Serial.println("Setup is ready!");
}

void loop() {
  int adcValue = 0;
  adcValue = analogRead(ADC_PIN); //read the ADC value on the pin
  float voltage = getVoltage(adcValue); //convert ADC value to voltage
  Serial.printf(
    "ADC value: %d, Voltage %.2f V\n", adcValue, voltage);


  if (!relayState && voltage < VOLTAGE_THRESHOLD - VOLTAGE_GIST) {
    // If it was OFF and gets dark enough, turn it ON
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(LED_PIN, HIGH);
    relayState = true;
    Serial.println("Relay Triggered: ON");
  }
  else if (relayState && voltage > VOLTAGE_THRESHOLD + VOLTAGE_GIST) {
    // If it was ON and gets bright enough, turn it OFF
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(LED_PIN, LOW);
    relayState = false;
    Serial.println("Relay Triggered: OFF");
  }

  delay(200); // Short delay for stability
}