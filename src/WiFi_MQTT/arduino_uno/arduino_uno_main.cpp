#include <SoftwareSerial.h>
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "Adafruit_Si7021.h"
#include "arduino_uno_main.h"
#include "PM2_5.h"

/* Si7021 Temp and Humidity sensor */
int read_Si7021_sensor(char *temperature, char *humidity);

Adafruit_Si7021 tempHumidSensor = Adafruit_Si7021();
char si_temp[10];
char si_humidity[10];

/* CO2 sensor - move to different file */
int read_CO2_sensor(float *CO2_ppm);
float get_CO2_measurement();
float CO2_data = 0.0;

/* PM2.5 sensor globals */
SoftwareSerial pmsSerial(2, 3);
struct pms5003data PMS_data;

/* SD card globals */
char buffer[DATA_BUFFER];
char timestamp[TIMESTAMP_BUFFER];
File myFile;
char TIME[10];
char DATE[20];

void setup()
{
  /* set reference for ADC */
  analogReference(DEFAULT);

  /* set up LED debuggers */
  pinMode(ERRLED, OUTPUT);
  pinMode(TESTLED, OUTPUT);
  digitalWrite(TESTLED, HIGH);
  digitalWrite(ERRLED, HIGH);

  /* begin communication with ESP8266 */
  Serial.begin(9600);

  /* wait for ESP8266 to connect to wifi */
  while (1)
  {
    while (!Serial.available())
      ;
    Serial.readBytesUntil('\n', buffer, DATA_BUFFER);
    if (!strcmp(buffer, ESP8266_READY))
      break;
  }
  digitalWrite(TESTLED, LOW);

  /* initialize SD card */
  if (!SD.begin(10))
  {
#ifdef MQTT_ON
    sprintf(buffer, "%s/error_messages SD_card_initialization_failed\n", NETWORK_ID);
    publish_data(buffer);
#endif
    digitalWrite(ERRLED, HIGH);
    while (1)
      ;
  }

  /* Initialize data files on SD cardsd */
  init_pms_file();
  init_co2_file();
  init_si7021_file();

  /* PM 2.5 sensor baud rate is 9600 */
  pmsSerial.begin(9600);

  /* Find Si7021 Sensor */
  if (!tempHumidSensor.begin())
  {
#ifdef MQTT_ON
    sprintf(buffer, "%s/error_messages Si7021\n", NETWORK_ID);
    publish_data(buffer);
#endif
  }

  digitalWrite(ERRLED, LOW);
  delay(1000);
}

void loop()
{
  /* request a timestamp from ESP8266 */
  get_timestamp();
  /* PM2.5 sensor readings available? */
  if (readPMSdata(&pmsSerial, &PMS_data))
  {
    /* reading data was successful - write to SD */
    digitalWrite(TESTLED, HIGH);
    write_PMS_to_sd();
    digitalWrite(TESTLED, LOW);
#ifdef MQTT_ON
    publish_PMS_data();
#endif
  }

  /* CO2 data available? */
  if (read_CO2_sensor(&CO2_data))
  {
    digitalWrite(TESTLED, HIGH);
    write_CO2_to_sd();
    digitalWrite(TESTLED, LOW);
  }

  /* Si7021 data available? */
  if (read_Si7021_sensor(si_temp, si_humidity))
  {
    digitalWrite(TESTLED, HIGH);
    write_Si7021_to_sd();
    digitalWrite(TESTLED, LOW);
  }
}

void write_PMS_to_sd()
{
  myFile = SD.open("pms.csv", FILE_WRITE);
  if (myFile)
  {
    snprintf(buffer, DATA_BUFFER, ",%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", PMS_data.pm10_standard, PMS_data.pm25_standard, PMS_data.pm100_standard, PMS_data.pm10_env, PMS_data.pm25_env, PMS_data.pm100_env, PMS_data.particles_03um, PMS_data.particles_05um, PMS_data.particles_10um, PMS_data.particles_25um, PMS_data.particles_50um, PMS_data.particles_100um);
    myFile.write(timestamp);
    myFile.write(buffer);
    myFile.close();
    memset(buffer, 0, sizeof(buffer));
    digitalWrite(ERRLED, LOW);
  }
  else
  {
#ifdef MQTT_ON
    sprintf(buffer, "%s/error_messages  SD_card_write_failed\n", NETWORK_ID);
    publish_data(buffer);
#endif
    digitalWrite(ERRLED, HIGH);
    digitalWrite(TESTLED, LOW);
  }
}

void write_CO2_to_sd()
{
  myFile = SD.open("co2.csv", FILE_WRITE);
  if (myFile)
  {
    snprintf(buffer, DATA_BUFFER, ",%s\n", String(CO2_data).c_str());
    myFile.write(timestamp);
    myFile.write(buffer);
    myFile.close();
    memset(buffer, 0, sizeof(buffer));
    digitalWrite(ERRLED, LOW);
  }
  else
  {
#ifdef MQTT_ON
    sprintf(buffer, "%s/error_messages  SD_card_write_failed\n", NETWORK_ID)
    publish_data(buffer);
#endif
    digitalWrite(ERRLED, HIGH);
    digitalWrite(TESTLED, LOW);
  }
}

void write_Si7021_to_sd()
{
  myFile = SD.open("si.csv", FILE_WRITE);
  if (myFile)
  {
    snprintf(buffer, DATA_BUFFER, ",%s,%s\n", si_temp, si_humidity);
    myFile.write(timestamp);
    myFile.write(buffer);
    myFile.close();
    memset(buffer, 0, sizeof(buffer));
    digitalWrite(ERRLED, LOW);
  }
  else
  {
#ifdef MQTT_ON
    sprintf(buffer, "%s/error_messages  SD_card_write_failed\n", NETWORK_ID)
        publish_data(buffer);
#endif
    digitalWrite(ERRLED, HIGH);
    digitalWrite(TESTLED, LOW);
  }
}

void init_csv_file(const char *filename, const char *csv_cols)
{
  uint8_t f_exists = 0;

  /* initialize PMS csv file */
  if (SD.exists(filename))
    f_exists = 1;

  myFile = SD.open(filename, FILE_WRITE);
  if (myFile)
  {
    if (f_exists)
    {
      myFile.write('\n');
      f_exists = 0;
    }
    else
    {
      myFile.write(csv_cols);
      //myFile.println(F("timestamp,PM_10,PM_25,PM_100,PM_10_env,pm_25_env,pm_100_env,num_particles_gt_03um,num_particles_gt_05um,num_particles_gt_10um,num_particles_gt_25um,num_particles_gt_50um,num_particles_gt_100um"));
    }
    myFile.close();
  }
  else
  {
#ifdef MQTT_ON
    sprintf(buffer, "%s/error_messages  Could_not_open_file_on_SD_card\n", NETWORK_ID)
        publish_data(buffer);
#endif
    digitalWrite(ERRLED, HIGH);
    while (1)
    {
      blink_err_led();
    }
  }
}

void init_pms_file()
{
  uint8_t f_exists = 0;

  if (SD.exists("pms.csv"))
    f_exists = 1;

  myFile = SD.open(F("pms.csv"), FILE_WRITE);
  if (myFile)
  {
    if (f_exists)
    {
      myFile.print('\n');
      f_exists = 0;
    }
    else
    {
      myFile.println(F("time,PM_10,PM_25,PM_100,PM_10_env,pm_25_env,pm_100_env,np_gt_03um,np_gt_05um,np_gt_10um,np_gt_25um,np_gt_50um,np_gt_100um"));
    }
    myFile.close();
  }
  else
  {
#ifdef MQTT_ON
    publish_data(String(NETWORK_ID) + String("error_messages"), String("Could_not_open_file_on_SD_card"));
#endif
    digitalWrite(ERRLED, HIGH);
    while (1)
    {
      blink_err_led();
    }
  }
}

void init_co2_file()
{
  uint8_t f_exists = 0;

  /* initialize CO2 csv file */
  if (SD.exists("co2.csv"))
    f_exists = 1;

  myFile = SD.open(F("co2.csv"), FILE_WRITE);
  if (myFile)
  {
    if (f_exists)
    {
      myFile.print('\n');
      f_exists = 0;
    }
    else
    {
      myFile.println(F("time,PPM"));
    }
    myFile.close();
  }
  else
  {
#ifdef MQTT_ON
    publish_data(String(NETWORK_ID) + String("error_messages"), String("Could_not_open_file_on_SD_card"));
#endif
    while (1)
    {
      blink_err_led();
    }
  }
}

void init_si7021_file()
{
  uint8_t f_exists = 0;

  /* initialize Si7021 csv file */
  if (SD.exists("si.csv"))
    f_exists = 1;
  myFile = SD.open(F("si.csv"), FILE_WRITE);
  if (myFile)
  {
    if (f_exists)
    {
      myFile.print('\n');
      f_exists = 0;
    }
    else
    {
      myFile.println(F("time,temp,humid"));
    }
    myFile.close();
  }
  else
  {
#ifdef MQTT_ON
    strcpy(buffer);
    publish_data(String(NETWORK_ID) + String("error_messages"), String("Could_not_open_si7021_file_on_SD_card"));
#endif
    while (1)
    {
      blink_err_led();
    }
  }
}

/* Communication with ESP8266 */
void get_timestamp()
{
  /* request timestamp */
  Serial.print(REQUEST_TIMESTAMP);

  /* First, get date*/
  Serial.readBytesUntil('\n', buffer, DATA_BUFFER);
  sprintf(DATE, "%s", buffer);
  memset(buffer, 0, sizeof(buffer));

  /* Then, get time */
  Serial.readBytesUntil('\n', buffer, DATA_BUFFER);
  sprintf(TIME, "%s", buffer);
  memset(buffer, 0, sizeof(buffer));

  /* Form timestamp buffer */
  sprintf(timestamp, "%s %s", DATE, TIME);
}

void publish_data(char *data)
{
  Serial.print(data);
  Serial.flush();
}

void publish_PMS_data()
{
  sprintf(buffer, "%s/PM_10 %d\n", NETWORK_ID, PMS_data.pm10_standard);
  publish_data(buffer);
  sprintf(buffer, "%s/PM_25 %d\n", NETWORK_ID, PMS_data.pm25_standard);
  publish_data(buffer);
  sprintf(buffer, "%s/PM_100 %d\n", NETWORK_ID, PMS_data.pm100_standard);
  publish_data(buffer);
  sprintf(buffer, "%s/PM_10_env %d\n", NETWORK_ID, PMS_data.pm10_env);
  publish_data(buffer);
  sprintf(buffer, "%s/PM_25_env %d\n", NETWORK_ID, PMS_data.pm25_env);
  publish_data(buffer);
  sprintf(buffer, "%s/PM_100_env %d\n", NETWORK_ID, PMS_data.pm100_env);
  publish_data(buffer);
  sprintf(buffer, "%s/particles_03um %d\n", NETWORK_ID, PMS_data.particles_03um);
  publish_data(buffer);
  sprintf(buffer, "%s/particles_05um %d\n", NETWORK_ID, PMS_data.particles_05um);
  publish_data(buffer);
  sprintf(buffer, "%s/particles_10um %d\n", NETWORK_ID, PMS_data.particles_10um);
  publish_data(buffer);
  sprintf(buffer, "%s/particles_25um %d\n", NETWORK_ID, PMS_data.particles_25um);
  publish_data(buffer);
  sprintf(buffer, "%s/particles_50um %d\n", NETWORK_ID, PMS_data.particles_50um);
  publish_data(buffer);
  sprintf(buffer, "%s/particles_100um %d\n", NETWORK_ID, PMS_data.particles_100um);
  publish_data(buffer);
}

/* Debugging functions */
void blink_err_led()
{
  digitalWrite(ERRLED, HIGH);
  delay(500);
  digitalWrite(ERRLED, LOW);
  delay(500);
}

/* Sensor Operations */
int read_Si7021_sensor(char *temperature, char *humidity)
{
  static unsigned long t0 = millis();
  if ((millis() - t0) <= 1000)
    return 0;
  t0 = millis();
  itoa(tempHumidSensor.readTemperature(), temperature, sizeof(temperature));
  itoa(tempHumidSensor.readHumidity(), humidity, sizeof(humidity));
  return 1;
}

int read_CO2_sensor(float *CO2_ppm)
{
  static unsigned long t0 = millis();
  if ((millis() - t0) <= 1000)
    return 0;
  t0 = millis();
  *CO2_ppm = get_CO2_measurement();
  return 1;
}

float get_CO2_measurement()
{
  /* read ADC value from CO2 sensor */
  int adcVal = analogRead(CO2_PIN);

  /* convert to voltage per CO2 sensor specs */
  float voltage = adcVal * (5000 / 1024.0);

  if (voltage == 0)
  {
    return -1;
  }
  else if (voltage < 400)
  {
    return -2;
  }
  else
  {
    float voltageDiference = voltage - 400;
    return (float)((voltageDiference * 50.0) / 16.0);
  }
}

/**************************************************************
* Function: celciusToFahrenheit
* ------------------------------------------------------------ 
* summary: Converts a given temperature in Celcius to its Fahrenheit 
* parameters: int8_t Temperature in Celcius
* return: int8_t Temperature in Fahrenheit
**************************************************************/
int8_t celciusToFahrenheit(int8_t c)
{
  return (c * 9.0) / 5.0 + 32;
}