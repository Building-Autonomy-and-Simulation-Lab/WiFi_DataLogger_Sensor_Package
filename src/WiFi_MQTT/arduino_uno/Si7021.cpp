#include "Si7021.h"
Adafruit_Si7021 tempHumidSensor = Adafruit_Si7021();

/* wrapper for starting the si7021 sensor */
bool start_si7021(){
    return tempHumidSensor.begin();
}

/* 
Reads the temperature and humidity data from the Si7021 sensor 
and copies it to target buffers
*/
void read_Si7021_sensor(char *temperature, size_t temp_size, char *humidity, size_t hum_size)
{
  float temp, hum;
  memset(temperature, 0, temp_size);
  memset(humidity, 0, hum_size);
  get_temperature(temp);
  get_humidity(hum);
  itoa(celcius_to_fahrenheit(temp), temperature, temp_size);
  itoa(hum, humidity, hum_size);
}

/*
Gets the temperature from the Si7021 sensor
    - will continue reading from the sensor until a valid
    value is read
*/
void get_temperature(float &temp){
  float tempRead = tempHumidSensor.readTemperature();
  while (tempRead == NAN){
    delay(50);
    tempRead = tempHumidSensor.readTemperature();
  }
  temp = tempRead;
}

/*
Gets the humidity from the Si7021 sensor
    - will continue reading from the sensor until a valid
    value is read
*/
void get_humidity(float &hum){
  float tempHum = tempHumidSensor.readHumidity();
  while (tempHum == NAN){
    delay(50);
    tempHum = tempHumidSensor.readHumidity();
  }
  hum = tempHum;
}

int8_t celcius_to_fahrenheit(int8_t c)
{
  return (c * 9.0) / 5.0 + 32;
}