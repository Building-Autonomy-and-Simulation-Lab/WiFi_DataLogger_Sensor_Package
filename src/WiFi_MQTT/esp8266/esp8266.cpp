#include "NTPClient.h"
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "esp8266.h"

// WiFi objects
IPAddress staticIP(192, 168, 0, 68);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);
IPAddress MQTTbroker(192, 168, 0, 190);

// For timestamp
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
char TIMESTAMP[25];

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", UTC_OFFSET_SECONDS, UPDATE_INTERVAL);

// Set up MQTT client
WiFiClient espClient;
PubSubClient MQTTclient(espClient);

// Topic variables
char TOPIC_MSG[MSG_BUFFER_SIZE];
char *TOPIC;
char *TOPIC_DATA;

void setup()
{
    pinMode(ERR_LED, OUTPUT); /* configure LED on ESP8266 for debugging use */
    digitalWrite(ERR_LED, HIGH);
    Serial.begin(9600);     /* begin serial monitoring */
#ifdef DEBUG
    Serial.println("started serial monitor");
#endif
    setup_Wifi(); /* configure wifi settings */
    timeClient.begin(); /* begin the NTP Client */
#ifdef MQTT_ON
    MQTTclient.setServer(MQTTbroker, 1883); /* start the MQTT client */
    //MQTTclient.setCallback(callback); /* function to handle subscription updates */
#endif
    digitalWrite(ERR_LED, LOW);
    char s_buff[5];
    while (1){
        Serial.print("done!\n");
        int bytes_read = Serial.readBytesUntil('\n', s_buff, 4);
        s_buff[bytes_read] = '\0';
        if (!strcmp(s_buff, ARDUINO_RDY))
            break;
    }
}

void loop()
{
    timeClient.update(); /* updates the time based on an interval (see esp8266.h for settings) */
#ifdef MQTT_ON
    if (!MQTTclient.connected()) /* connection broken? */
         reconnect_to_broker();
    MQTTclient.loop();
#endif
    if (Serial.available() > 0) /* is data is available from Arduino? */
    {
        char s_buff[100];
        int bytes_read = Serial.readBytesUntil('\n', s_buff, sizeof(s_buff) - 1); /* read arduino data from serial port */
        s_buff[bytes_read] = '\0';
        if (!strcmp(s_buff, TIMESTAMP_REQUEST)){
            set_timestamp();
            print_timestamp();
        }
        else if (get_topic_and_topic_msg(s_buff)) /* topic name and data legible? */
        {
            MQTTclient.publish(TOPIC, TOPIC_MSG);  /* publish to topic */
        }
    }
}

void setup_Wifi()
{
    delay(3000);
    digitalWrite(ERR_LED, HIGH);
    WiFi.hostname(HOSTNAME);
    if (!WiFi.config(staticIP, gateway, subnet, primaryDNS, secondaryDNS))
    {
#ifdef DEBUG
        Serial.println("STA failed to configure");
#endif
        error_blink_leds(200);
    }

#ifdef DEBUG
    Serial.print("Connecting...");
#endif
    WiFi.begin(SSID, SSID_PW);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
#ifdef DEBUG
        Serial.print(".");
#endif
    }
    digitalWrite(ERR_LED, LOW);
#ifdef DEBUG
    Serial.print("Connected!\nIP Address: ");
    Serial.println(WiFi.localIP());
#endif
}

/* function used to reconnect to MQTT broker in case of a lost cnxn */
void reconnect_to_broker()
{
    // Loop until we're reconnected
    while (!MQTTclient.connected())
    {
// Create a random client ID (may want to keep this static, actually. a good ID could be the ESP8266's MAC address)
#ifdef DEBUG
        Serial.print("Attempting MQTT connection...");
#endif
        char clientId[] = "ESP8266Client-1";
        //int rand_id = random(0xffff), HEX); // replace with MAC address?

        // Attempt to connect
        if (MQTTclient.connect(clientId))
        {
#ifdef DEBUG
            Serial.println("connected");
#endif
            digitalWrite(ERR_LED, LOW);
        }
        else
        {
#ifdef DEBUG
            Serial.print("failed, rc=");
            Serial.print(MQTTclient.state());
            Serial.println(" try again in 5 seconds");
#endif
            error_blink_leds(400); // blink LEDS 4 times with 400ms as the delay
        }
    }
}

int get_topic_and_topic_msg(char *ser_data)
{

    TOPIC = strtok(ser_data, " "); /* data comes in from arduino as "DHN/<network #>/<node id>/<topic name> <data>"; delim is space */
    if (TOPIC == NULL)
        return 0;

    TOPIC_DATA = strtok(NULL, " ");
    if (TOPIC_DATA == NULL)
    {
#ifdef DEBUG
        Serial.print("Error: missing data point for topic ");
        Serial.print(TOPIC);
        Serial.println("\nPublishing terminated.");
#endif
        return 0;
    }
    else
    {
        if (strcmp(TOPIC, "error_messages") == 0){
            snprintf(TOPIC_MSG, sizeof(TOPIC_MSG), "%s - %s", TIMESTAMP, ser_data);
        } else {
            snprintf(TOPIC_MSG, sizeof(TOPIC_MSG), "%s - %s", TIMESTAMP, TOPIC_DATA); /* put a timestamp on data */
        }
        return 1;
    }
}

void set_timestamp()
{
    memset(TIMESTAMP, 0, sizeof(TIMESTAMP));
    sprintf(TIMESTAMP, "%s %s", timeClient.getFormattedDate().c_str(), timeClient.getFormattedTime().c_str());
}

void print_timestamp()
{
    Serial.print(TIMESTAMP);
    Serial.print('\n');
    Serial.flush();
}

void error_blink_leds(int ms)
{
    digitalWrite(ERR_LED, HIGH);
    delay(ms);
    digitalWrite(ERR_LED, LOW);
    delay(ms);
    digitalWrite(ERR_LED, HIGH);
    delay(ms);
    digitalWrite(ERR_LED, LOW);
    delay(ms);
    digitalWrite(ERR_LED, HIGH);
    delay(ms);
    digitalWrite(ERR_LED, LOW);
    delay(ms);
    digitalWrite(ERR_LED, HIGH);
    delay(ms);
    digitalWrite(ERR_LED, LOW);
    delay(ms);
    digitalWrite(ERR_LED, HIGH);
}