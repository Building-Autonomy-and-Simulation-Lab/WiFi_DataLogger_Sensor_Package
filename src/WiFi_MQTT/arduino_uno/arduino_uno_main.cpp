#include <SoftwareSerial.h>
#include <Arduino.h>
#include <SD.h>
#include "arduino_uno_main.h"
#include "PM2_5.h"
#include "Si7021.h"

/* Si7021 Temp and Humidity sensor */

char si_temp[10];
char si_humidity[10];

/* CO2 sensor - move to different file */
void get_CO2_measurement(float &CO2_ppm);
float CO2_data = 0.0;

/* PM2.5 sensor globals */
SoftwareSerial pmsSerial(2, 3);
struct pms5003data PMS_data;

/* SD card globals */
char buffer[DATA_BUFFER];
char timestamp[TIMESTAMP_BUFFER];
File myFile;

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
    if (!strcmp(buffer, ESP8266_READY)){
      Serial.print(ARDUINO_RDY);
      break;
    }
  }
  digitalWrite(TESTLED, LOW);

  /* initialize SD card */
  if (!SD.begin(10))
  {
#ifdef MQTT_ON
    sprintf(buffer, "%s/%s SD_card_init", NETWORK_ID, ERR_TOPIC);
    publish_data(buffer);
    memset(buffer, 0, sizeof(buffer));
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
  if (!start_si7021())
  {
#ifdef MQTT_ON
    sprintf(buffer, "%s/%s Si7021_init", NETWORK_ID, ERR_TOPIC);
    publish_data(buffer);
    memset(buffer, 0, sizeof(buffer));
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
    /* read and write data to SD */
    digitalWrite(TESTLED, HIGH);
    write_PMS_to_sd();
    get_CO2_measurement(CO2_data);
    write_CO2_to_sd();
    read_Si7021_sensor(si_temp, sizeof(si_temp), si_humidity, sizeof(si_humidity));
    write_Si7021_to_sd();
    digitalWrite(TESTLED, LOW);
#ifdef MQTT_ON
    publish_PMS_data();
#endif
  }
}

void write_PMS_to_sd()
{
  myFile = SD.open(F("pms.csv"), FILE_WRITE);
  if (myFile)
  {
    snprintf(buffer, DATA_BUFFER, ",%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", PMS_data.pm10_standard, PMS_data.pm25_standard, PMS_data.pm100_standard, PMS_data.pm10_env, PMS_data.pm25_env, PMS_data.pm100_env, PMS_data.particles_03um, PMS_data.particles_05um, PMS_data.particles_10um, PMS_data.particles_25um, PMS_data.particles_50um, PMS_data.particles_100um);
    myFile.write(timestamp);
    myFile.write(buffer);
    myFile.close();
    memset(buffer, 0, DATA_BUFFER);
    digitalWrite(ERRLED, LOW);
  }
  else
  {
#ifdef MQTT_ON
    sprintf(buffer, "%s/%s pms_write\n", NETWORK_ID, ERR_TOPIC);
    publish_data(buffer);
    memset(buffer, 0, DATA_BUFFER);
#endif
    digitalWrite(ERRLED, HIGH);
    digitalWrite(TESTLED, LOW);
  }
}

void write_CO2_to_sd()
{
  if (CO2_data <= 0) return;
  myFile = SD.open(F("co2.csv"), FILE_WRITE);
  if (myFile)
  {
    snprintf(buffer, DATA_BUFFER, ",%s\n", String(CO2_data).c_str());
    myFile.write(timestamp);
    myFile.write(buffer);
    myFile.close();
    memset(buffer, 0, DATA_BUFFER);
    digitalWrite(ERRLED, LOW);
  }
  else
  {
#ifdef MQTT_ON
    sprintf(buffer, "%s/%s co2_write", NETWORK_ID, ERR_TOPIC);
    publish_data(buffer);
    memset(buffer, 0, DATA_BUFFER);
#endif
    digitalWrite(ERRLED, HIGH);
    digitalWrite(TESTLED, LOW);
  }
}

void write_Si7021_to_sd()
{
  myFile = SD.open(F("si.csv"), FILE_WRITE);
  if (myFile)
  {
    snprintf(buffer, DATA_BUFFER, ",%s,%s\n", si_temp, si_humidity);
    myFile.write(timestamp);
    myFile.write(buffer);
    myFile.close();
    memset(buffer, 0, DATA_BUFFER);
    digitalWrite(ERRLED, LOW);
  }
  else
  {
#ifdef MQTT_ON
    sprintf(buffer, "%s/%s si7021_write", NETWORK_ID, ERR_TOPIC);
    publish_data(buffer);
    memset(buffer, 0, DATA_BUFFER);
#endif
    digitalWrite(ERRLED, HIGH);
    digitalWrite(TESTLED, LOW);
  }
}

void init_pms_file()
{
  uint8_t f_exists = 0;

  if (SD.exists(F("pms.csv")))
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
    sprintf(buffer, "%s/%s pms_file_init", NETWORK_ID, ERR_TOPIC);
    publish_data(buffer);
    memset(buffer, 0, DATA_BUFFER);
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
  if (SD.exists(F("co2.csv")))
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
    sprintf(buffer, "%s/%s co2_file_init", NETWORK_ID, ERR_TOPIC);
    publish_data(buffer);
    memset(buffer, 0, DATA_BUFFER);
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
  if (SD.exists(F("si.csv")))
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
    sprintf(buffer, "%s/%s si7021_init", NETWORK_ID, ERR_TOPIC);
    publish_data(buffer);
    memset(buffer, 0, DATA_BUFFER);
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
  memset(timestamp, 0, TIMESTAMP_BUFFER);
  Serial.print(REQUEST_TIMESTAMP);
  Serial.readBytesUntil('\n', timestamp, TIMESTAMP_BUFFER);
}

void publish_data(char *data)
{
  Serial.print(data);
  Serial.print('\n');
  Serial.flush();
}

void publish_PMS_data()
{
  sprintf(buffer, "%s/PM_10 %d", NETWORK_ID, PMS_data.pm10_standard);
  publish_data(buffer);
  memset(buffer, 0, DATA_BUFFER);
  sprintf(buffer, "%s/PM_25 %d", NETWORK_ID, PMS_data.pm25_standard);
  publish_data(buffer);
  memset(buffer, 0, DATA_BUFFER);
  sprintf(buffer, "%s/PM_100 %d", NETWORK_ID, PMS_data.pm100_standard);
  publish_data(buffer);
  memset(buffer, 0, DATA_BUFFER);
  sprintf(buffer, "%s/PM_10_env %d", NETWORK_ID, PMS_data.pm10_env);
  publish_data(buffer);
  memset(buffer, 0, DATA_BUFFER);
  sprintf(buffer, "%s/PM_25_env %d", NETWORK_ID, PMS_data.pm25_env);
  publish_data(buffer);
  memset(buffer, 0, DATA_BUFFER);
  sprintf(buffer, "%s/PM_100_env %d", NETWORK_ID, PMS_data.pm100_env);
  publish_data(buffer);
  memset(buffer, 0, DATA_BUFFER);
  sprintf(buffer, "%s/gt_03um %d", NETWORK_ID, PMS_data.particles_03um);
  publish_data(buffer);
  memset(buffer, 0, DATA_BUFFER);
  sprintf(buffer, "%s/gt_05um %d", NETWORK_ID, PMS_data.particles_05um);
  publish_data(buffer);
  memset(buffer, 0, DATA_BUFFER);
  sprintf(buffer, "%s/gt_10um %d", NETWORK_ID, PMS_data.particles_10um);
  publish_data(buffer);
  memset(buffer, 0, DATA_BUFFER);
  sprintf(buffer, "%s/gt_25um %d", NETWORK_ID, PMS_data.particles_25um);
  publish_data(buffer);
  memset(buffer, 0, DATA_BUFFER);
  sprintf(buffer, "%s/gt_50um %d", NETWORK_ID, PMS_data.particles_50um);
  publish_data(buffer);
  memset(buffer, 0, DATA_BUFFER);
  sprintf(buffer, "%s/gt_100um %d", NETWORK_ID, PMS_data.particles_100um);
  publish_data(buffer);
  memset(buffer, 0, DATA_BUFFER);
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
void get_CO2_measurement(float &CO2_ppm)
{
  /* read ADC value from CO2 sensor */
  int adcVal = analogRead(CO2_PIN);

  /* convert to voltage per CO2 sensor specs */
  float voltage = adcVal * (5000 / 1024.0);

  if (voltage == 0)
  {
    CO2_ppm = -1.0f;
  }
  else if (voltage < 400)
  {
    CO2_ppm = -2.0f;
  }
  else
  {
    float voltageDiference = voltage - 400;
    CO2_ppm = (float)((voltageDiference * 50.0) / 16.0);
  }
}