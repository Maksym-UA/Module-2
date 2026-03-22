
#include <Arduino.h>
#include <esp_task_wdt.h>

//Configuration
const uint8_t fanPin = 16;
uint32_t workSec = 5;
uint32_t totalSec = 20;


volatile uint32_t secondsCounter = 0;
volatile bool isFanOn = false;
volatile bool needLog = false;

//ESP32-S3 hardware timer
hw_timer_t * timer = NULL;

void IRAM_ATTR onTimer() {
  secondsCounter++;

  if (secondsCounter <= workSec) {
    if (!isFanOn) {
      isFanOn = true;
      needLog = true;
    }
  } else if (secondsCounter < totalSec) {
    if (isFanOn) {
      isFanOn = false;
      needLog = true;
    }
  } else {
    secondsCounter = 0;
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