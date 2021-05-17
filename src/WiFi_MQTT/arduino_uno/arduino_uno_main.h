/* constants */
#define NETWORK_ID "DHN/0/0"
#define DONE "done!"
#define ERRLED 7
#define TESTLED 5
#define TIMESTAMP_BUFFER 30
#define DATA_BUFFER 100
#define REQUEST_TIMESTAMP "t\n"
#define ESP8266_READY "done!"
#define CO2_PIN A5
//#define MQTT_ON /* uncomment to enable publishing to MQTT topics */

/* prototypes */
void publish_PMS_data();
void publish_data(char *data);
void write_PMS_to_sd();
void write_CO2_to_sd();
void write_Si7021_to_sd();
void init_pms_file();
void init_co2_file();
void init_si7021_file();
void get_timestamp();
void blink_err_led();