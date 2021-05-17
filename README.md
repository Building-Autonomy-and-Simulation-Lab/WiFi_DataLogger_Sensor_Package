# WiFi + Arduino Sensor Package 

The exponential growth of wireless communications markets around the world has allowed users from all corners of the globe to share data almost instantaneously. However, managing the collection and transmission of data from multiple devices connected in small, confined networks presents major challenges. The futuristic ideal of having millions of interconnected "things" (inanimate objects, devices, communities, and environments) that have the ability to sense, communicate, network, and produce mass amounts of data that can be utilized in a novel way is known as the Internet of Things (IoT). Automation, environmental analysis, and health and safety awareness are large areas of study that can benefit from IoT networks, but issues with compatibility, versatility, and cost inhibit implementation of such networks.

The purpose of this project is to create an inexpensive, robust, low-power IoT sensor network to serve as an easily implementable model for individuals, communities, and cities. Additionally, the network can serve as a tool within other areas of research for experimentation and QA analysis. The foundation of this system was successfully created using Arduino-based processing in tandem with packet radios that communicate over sub-GHz frequency bands. Ethernet data packet management was done using MQ Telemetry Transport (MQTT) and data visualization was performed with Grafana and MATLAB. This network also utilizes InfluxDB to store and manage data effectively.


## Contents
- [Features](#features)
- [Quick start](#quickstart)
  - [Necessary hardware](#necessaryhardware)
  - [Necessary tools](#necessarytools)
  - [Hardware setup](#hardwaresetup)
  - [Network setup](#networksetup)
- [Research Analysis](#ra)
  - [Cost Scaling](#cost)
- [Releases](#releases)
- [Contributors](#contributors)

## Features <a name="features"></a>
The completed network has the following features:
- Modular addition and modification of sensors,
- WiFi connectivity for accurate timestamping of data,
- Support for the MQTT protocol,
- Local data storage

## Quick start <a name="quickstart"></a>
### Necessary hardware <a name="necessaryhardware"></a>
The following hardware is necessary to set up the network and connect one node:
- (1) ESP8266 + Arduino Uno microcontroller
- (1) Gravity Infrared CO2 Sensor
- (1) Plantower PMS5003 Particle Concentration Sensor
- (1) Si7021 Temperature and Humidity Sensor
- (1) MicroSD Card Driver Module
- (1) MicroSD Card <= 16GB
- Solid core wires
- Solder

### Necessary tools and resources <a name="necessarytools"></a>
- Arduino/USB cable
For uploading sketches to the microcontroller
- Internet connection
For acquiring accurate timestamps and for MQTT capabilities


### Hardware setup <a name="necessarytools"></a>

The following image illustrates the necessary wire connections for the sensor package:
![Sensor Package Wiring](/images/sensor_package_wiring.png)

### Network setup <a name="networksetup"></a>

Below are the steps to initialize the server, gateway and one sensor node.

1. **MQTT client setup**

   Instructions on how to set up the MQTT client on the ESP8266 and ATMega will be written soon.


2. **MQTT Server setup.**
   Instructions on how to set up the MQTT server will be written soon.


## Research Analysis <a name="ra"></a>
### Cost Scaling <a name="cost"></a>
The following table documents the cost of the core of the network - the Ethernet and gateway nodes.

| Component                                                                   | Price   |
|-----------------------------------------------------------------------------|---------|
| [Arduino Uno + Wifi]("https://www.amazon.com/dp/B0834JQ5Y5?ref_=pe_1815430_211938580")                                  | $11.99 |
| [Gravity: Analog Infrared CO2 Sensor]("https://www.dfrobot.com/product-1549.html") | $2.15  |
| [PM2.5 Air Quality Sensor]("https://www.adafruit.com/product/3686")                 | $0.69   |
| [Si7021 Temperature & Humidity Sensor]("https://www.adafruit.com/product/3251")                     | $2.08   |
| [Solid core wire]                          | $0.48  |
| [MicroSD Card Driver Module (5pcs)]("https://www.amazon.com/Adapter-Reader-interface-driver-Arduino/dp/B01MSNX0TW/") | $9.28 |
| Total (+ shipping)                                               | $13.67 |



The final table documents how cost scales as _n_ nodes are added to the network.

| Nodes | Cost      |
|-------|-----------|
| 1     | $13.67    |
| 10    | $106.28   |
| 100   | $1210.18  |
| 500   | $6893.38  |


## Releases <a name="releases"></a>
Not currently released.