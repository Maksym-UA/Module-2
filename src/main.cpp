
#include <Arduino.h>
#include <esp_task_wdt.h>
#include <atomic>

//Configuration
const uint8_t fanPin = 16;
const uint32_t workSec = 5;
uint32_t totalSec = 20;
std::atomic<uint32_t> secondsCounter{0};// Initialize atomic counter to 0

enum class FanState : uint8_t {
  Off,
  On,
};

volatile bool isFanOn = false;
volatile bool needLog = false;
volatile FanState fanState = FanState::Off;

//ESP32-S3 hardware timer
hw_timer_t * timer = NULL;

void IRAM_ATTR onTimer() {
  const uint32_t currentSecond = secondsCounter.fetch_add(1, std::memory_order_relaxed) + 1;

  switch (fanState) {
    case FanState::Off:
      if (currentSecond <= workSec) {
        fanState = FanState::On;
        isFanOn = true;
        needLog = true;
      } else if (currentSecond >= totalSec) {
        secondsCounter.store(0, std::memory_order_relaxed);
      }
      break;

    case FanState::On:
      if (currentSecond > workSec) {
        fanState = FanState::Off;
        isFanOn = false;
        needLog = true;
      }
      break;
  }

  digitalWrite(fanPin, isFanOn);
}

void setup() {
  Serial.begin(115200);
  pinMode(fanPin, OUTPUT);

  //Set up watchdog with 3 sec timeout for the current task
  esp_task_wdt_init(3, true);
  esp_task_wdt_add(NULL);

  // ESP32-S3 frequency 80MHz. Prescaler 80  gives 1,000,000 ticks/sec.
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);

  // Set actuation every 1,000,000 microsecond (1 second)
  timerAlarmWrite(timer, 1000000, true);
  timerAlarmEnable(timer);

  Serial.println("ESP32-S3 Fan Controller Started");
}

void loop() {
  //Reset the watchdog timer to prevent reset
  esp_task_wdt_reset();
   if (needLog) {
    needLog = false; // Скидаємо прапорець після логування
    Serial.printf("Статус вентилятора: %s | Тривалість: %u сек\n",
      isFanOn ? "ON" : "OFF", isFanOn ? workSec : (totalSec - workSec));
   }

  delay(100);
}