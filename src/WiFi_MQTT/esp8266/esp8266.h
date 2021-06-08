#define UTC_OFFSET_SECONDS -18000 /* offset for Atlantic Time (daylight savings) */
#define UPDATE_INTERVAL 360000 /* ms, updates every minute */
#define MSG_BUFFER_SIZE 100 /* bytes, for PubSubClient */
#define DATA_BUFFER_SIZE 200
#define ERR_LED 12
#define ARDUINO_RX 2
#define ARDUINO_TX 4
#define TIMESTAMP_REQUEST "t"
#define ARDUINO_RDY "k"
//#define MQTT_ON /* uncomment to enable MQTT communication */
//#define DEBUG /* uncomment for debug prints */

/* WiFi settings */
#define SSID "the-name-of-the-wifi-network"
#define SSID_PW "wifi-passowrd"
#define HOSTNAME "ESP8266-1"

/* prototypes */
void error_blink_leds(int ms);
void setup_Wifi();
void reconnect_to_broker();
int get_topic_and_topic_msg(char *ser_data);
void set_timestamp();
void print_timestamp();
void print_date();
void print_time();