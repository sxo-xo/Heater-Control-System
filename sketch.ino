#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define RELAY_PIN       26
#define HEAT_LED_PIN    13
#define IDLE_LED_PIN    25
#define HEATING_LED_PIN 33
#define BUZZER_PIN      27

const int BAR_PINS[10] = {23, 19, 17, 16, 15, 14, 12, 5, 4, 2};

// Initial temperature from JSON
const float INITIAL_TEMP     = 0;
const int   INITIAL_DELAY_MS = 10000;  // 10 seconds
const int   SIM_STEP_MS      = 2000;   // 2 seconds

const float STAB_TEMP        = 30.0;
const float TARGET_TEMP      = 40.0;
const float OVERHEAT_TEMP    = 40.0;

LiquidCrystal_I2C lcd(0x27, 16, 2);

static float currentTemp = INITIAL_TEMP;
static String currentState = "";

void setIndicators(bool heaterOn, bool heatLed, bool idleLed, bool buzzerOn) {
  digitalWrite(RELAY_PIN,       heaterOn ? LOW  : HIGH);
  digitalWrite(HEAT_LED_PIN,    heatLed);
  digitalWrite(IDLE_LED_PIN,    idleLed);
  digitalWrite(HEATING_LED_PIN, heaterOn);
  if (buzzerOn) tone(BUZZER_PIN, 500);
  else noTone(BUZZER_PIN);
}

int heaterControlBar(float temp) {
  int segments = map(temp, 0, (int)TARGET_TEMP, 10, 0);
  segments = constrain(segments, 0, 10);
  for (int i = 0; i < 10; i++) {
    digitalWrite(BAR_PINS[i], i < segments ? HIGH : LOW);
  }
  return segments;
}

void simTempTask(void *pvParameters) {
  vTaskDelay(pdMS_TO_TICKS(INITIAL_DELAY_MS));
  while (1) {
    currentTemp += 2.0;
    vTaskDelay(pdMS_TO_TICKS(SIM_STEP_MS));
  }
}

void controlTask(void *pvParameters) {
  float lastTemp = -999;
  String lastState = "";

  while (1) {
    // Determine state and update indicators
    if (currentTemp > OVERHEAT_TEMP) {
      setIndicators(false, true, false, true);
      currentState = "OVERHEAT";
    } else if (currentTemp >= TARGET_TEMP) {
      setIndicators(false, false, true, false);
      currentState = "TARGET_OK";
    } else if (currentTemp >= STAB_TEMP) {
      setIndicators(true, false, true, false);
      currentState = "STABILIZING";
    } else if (currentTemp > INITIAL_TEMP) {
      setIndicators(true, false, false, false);
      currentState = "HEATING";
    } else {
      setIndicators(false, false, true, false);
      currentState = "IDLE";
    }

    heaterControlBar(currentTemp);  // Update LED bar graph

    // Update LCD only on change
    if (fabs(currentTemp - lastTemp) > 0.1 || currentState != lastState) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Temp:"); lcd.print(currentTemp,1); lcd.print("C");
      lcd.setCursor(0, 1);
      lcd.print(currentState);
      lastTemp = currentTemp;
      lastState = currentState;
    }

    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

void setup() {
  lcd.init();
  lcd.backlight();

  // Welcome screen
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Welcome");
  lcd.setCursor(2, 1);
  lcd.print("Starting...");
  delay(INITIAL_DELAY_MS);
  lcd.clear();

  pinMode(RELAY_PIN,       OUTPUT);
  pinMode(HEAT_LED_PIN,    OUTPUT);
  pinMode(IDLE_LED_PIN,    OUTPUT);
  pinMode(HEATING_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN,      OUTPUT);

  digitalWrite(RELAY_PIN,       HIGH);
  digitalWrite(HEAT_LED_PIN,    LOW);
  digitalWrite(IDLE_LED_PIN,    LOW);
  digitalWrite(HEATING_LED_PIN, LOW);
  digitalWrite(BUZZER_PIN,      LOW);

  for (int i = 0; i < 10; i++) {
    pinMode(BAR_PINS[i], OUTPUT);
    digitalWrite(BAR_PINS[i], LOW);
  }

  xTaskCreate(simTempTask,   "SimTemp",  2048, NULL, 1, NULL);
  xTaskCreate(controlTask,   "Control",  4096, NULL, 2, NULL);
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(100));
}
