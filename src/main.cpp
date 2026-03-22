
#include <Arduino.h>
#include <esp_task_wdt.h>

//Configuration
uint32_t workSec = 10; //сек
uint32_t totalSec = 30;

// Змінні стану
volatile uint32_t secondsCounter = 0;
volatile bool isFanOn = false;
volatile bool needLog = false;

// Об'єкт апаратного таймера
hw_timer_t * timer = NULL;

// Функція переривання (працює в RAM для швидкості)
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
    secondsCounter = 0; // Скидання циклу
  }
  
  // Керування піном безпосередньо в перериванні
  digitalWrite(fanPin, isFanOn);
}

void setup() {
  Serial.begin(115200);
  pinMode(fanPin, OUTPUT);

  // 1. Налаштування Watchdog (ESP32-S3 має специфічне API)
  // Встановлюємо тайм-аут 5 секунд для поточної задачі
  esp_task_wdt_init(5, true); 
  esp_task_wdt_add(NULL);

  // 2. Апаратний таймер
  // Частота ESP32-S3 зазвичай 80MHz. Дільник 80 дає 1,000,000 тіків на сек.
  timer = timerBegin(0, 80, true); 
  timerAttachInterrupt(timer, &onTimer, true);
  
  // Встановлюємо спрацювання кожні 1,000,000 мікросекунд (1 сек)
  timerAlarmWrite(timer, 1000000, true); 
  timerAlarmEnable(timer);

  Serial.println("ESP32-S3 Fan Controller Started");
}

void loop() {
  // 3. Скидання Watchdog
  esp_task_wdt_reset();

  // 4. Логування
  if (needLog) {
    needLog = false;
    Serial.printf("[LOG] Статус вентилятора: %s | Час циклу: %u сек\n", 
                  isFanOn ? "УВІМКНЕНО" : "ВИМКНЕНО", secondsCounter);
  }

  // loop() може "спати" або виконувати інші задачі (напр. Wi-Fi)
  delay(100); 
}