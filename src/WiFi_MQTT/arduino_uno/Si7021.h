#pragma once
#include "Adafruit_Si7021.h"

bool start_si7021();
void get_temperature(float &temp);
void get_humidity(float &hum);
void read_Si7021_sensor(char *temperature, size_t temp_size, char *humidity, size_t hum_size);
int8_t celcius_to_fahrenheit(int8_t c);