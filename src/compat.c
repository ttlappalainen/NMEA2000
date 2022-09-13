/**
 * @file compat.h
 * @author Nicklas Borjesson(nicklasb@gmail.com)
 * @brief Implementation of an ESP-IDF compatibility layer for the NMEA2000 library
 * @version 0.1
 * @date 2022-09-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifdef ESP_PLATFORM

#include <esp_timer.h>
#include <math.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

unsigned long millis() {
    return esp_timer_get_time()/1000;
}

double round(double val) {
  return val >= 0
      ? floor(val + 0.5)
      : ceil(val - 0.5);
}

void delay(unsigned long ms) {
    vTaskDelay(ms/portTICK_PERIOD_MS);
}

#endif