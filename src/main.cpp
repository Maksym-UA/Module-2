
#include <Arduino.h>

struct Config {
  static constexpr uint8_t LedGreen = 16;
  static constexpr uint8_t LedYellow = 17;
  static constexpr uint8_t LedRed = 18;
  static constexpr uint16_t BaudRate = 115200;

  static constexpr uint16_t GreenDurationMs = 5000;
  static constexpr uint16_t GreenBlinkDurationMs = 3000;
  static constexpr uint16_t BlinkIntervalMs = 500;
  static constexpr uint16_t YellowDurationMs = 2000;
  static constexpr uint16_t RedDurationMs = 5000;
  static constexpr uint16_t RedYellowDurationMs = 2000;
};

// State machine for traffic light control
enum class TrafficStage {
  Green,
  GreenBlink,
  Yellow,
  Red,
  RedYellow
};

//set initial state and timing variables
TrafficStage currentStage = TrafficStage::Green;
unsigned long stageStartMs = 0;
unsigned long blinkToggleStartMs = 0;
unsigned long blinkTotalStartMs = 0;

// Helper function to set the state of the traffic lights
void setLights(bool greenOn, bool yellowOn, bool redOn) {
  digitalWrite(Config::LedGreen, greenOn ? HIGH : LOW);
  digitalWrite(Config::LedYellow, yellowOn ? HIGH : LOW);
  digitalWrite(Config::LedRed, redOn ? HIGH : LOW);
}

// Function to transition to the next stage and set the appropriate lights
void enterStage(TrafficStage nextStage, unsigned long now) {
  currentStage = nextStage;
  stageStartMs = now;

  switch (currentStage) {
    case TrafficStage::Green:
      setLights(true, false, false);
      break;

    case TrafficStage::GreenBlink:
      setLights(true, false, false);
      blinkToggleStartMs = now;
      blinkTotalStartMs = now;
      break;

    case TrafficStage::Yellow:
      setLights(false, true, false);
      break;

    case TrafficStage::Red:
      setLights(false, false, true);
      break;

    case TrafficStage::RedYellow:
      setLights(false, true, true);
      break;
  }
}

void setup() {
  Serial.begin(Config::BaudRate);

  pinMode(Config::LedGreen, OUTPUT);
  pinMode(Config::LedYellow, OUTPUT);
  pinMode(Config::LedRed, OUTPUT);

  enterStage(TrafficStage::Green, millis());
}

void loop() {
  const unsigned long now = millis();

  switch (currentStage) {
    case TrafficStage::Green:
      if (now - stageStartMs >= Config::GreenDurationMs) {
        enterStage(TrafficStage::GreenBlink, now);
      }
      break;

    case TrafficStage::GreenBlink:
      if (now - blinkToggleStartMs >= Config::BlinkIntervalMs) {
        blinkToggleStartMs = now;
        digitalWrite(Config::LedGreen, !digitalRead(Config::LedGreen));
      }

      if (now - blinkTotalStartMs >= Config::GreenBlinkDurationMs) {
        enterStage(TrafficStage::Yellow, now);
      }
      break;

    case TrafficStage::Yellow:
      if (now - stageStartMs >= Config::YellowDurationMs) {
        enterStage(TrafficStage::Red, now);
      }
      break;

    case TrafficStage::Red:
      if (now - stageStartMs >= Config::RedDurationMs) {
        enterStage(TrafficStage::RedYellow, now);
      }
      break;

    case TrafficStage::RedYellow:
      if (now - stageStartMs >= Config::RedYellowDurationMs) {
        enterStage(TrafficStage::Green, now);
      }
      break;
  }
}