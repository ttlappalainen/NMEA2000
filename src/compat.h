/**
 * @file compat.h
 * @author Nicklas Borjesson(nicklasb@gmail.com)
 * @brief ESP-IDF compatibility layer for the NMEA2000 library
 * @version 0.1
 * @date 2022-09-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef ESP_PLATFORM

uint32_t millis();
double round(double val);
void delay(unsigned long ms);

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif