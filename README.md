# WiFi + Arduino Sensor Package 

The exponential growth of wireless communications markets around the world has allowed users from all corners of the globe to share data almost instantaneously. However, managing the collection and transmission of data from multiple devices connected in small, confined networks presents major challenges. The futuristic ideal of having millions of interconnected "things" (inanimate objects, devices, communities, and environments) that have the ability to sense, communicate, network, and produce mass amounts of data that can be utilized in a novel way is known as the Internet of Things (IoT). Automation, environmental analysis, and health and safety awareness are large areas of study that can benefit from IoT networks, but issues with compatibility, versatility, and cost inhibit implementation of such networks.

The purpose of this project is to create an inexpensive, robust, low-power IoT sensor network to serve as an easily implementable model for individuals, communities, and cities. Additionally, the network can serve as a tool within other areas of research for experimentation and QA analysis. Wireless data packet management was done using MQ Telemetry Transport (MQTT).


## Contents
- [Features](#features)
- [Quick start](#quickstart)
  - [Necessary hardware](#necessaryhardware)
  - [Supported Sensors](#supportedsensors)
  - [Necessary tools](#necessarytools)
  - [Hardware setup](#hardwaresetup)
  - [Network setup](#networksetup)
  - [Uploading Sketches](#uploadsketches)
    - [Prerequisites](#uploadprequisite)
    - [Modes of Serial Operation](#serialmodes)
    - [Uploading a Sketch to the ESP8266](#uploadesp)
    - [Uploading a Sketch to the ATMega](#uploadatmega)
  - [Running the Sensor Package](#runpackage)
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
- (1) MicroSD Card Driver Module
- (1) MicroSD Card <= 16GB

### Supported Sensors <a name="supportedsensors"></a>
The following sensors are supported by the sensor package:
- Gravity Infrared CO2 Sensor
- Plantower PMS5003 Particle Concentration Sensor
- Si7021 Temperature and Humidity Sensor

Of course, at least one sensor should be used in the sensor package. However, all three sensors may be used at once.

### Necessary tools and resources <a name="necessarytools"></a>
- Arduino/USB cable, for uploading sketches to the microcontroller
- Internet connection, for acquiring accurate timestamps and for MQTT capabilities


### Hardware setup <a name="necessarytools"></a>

The following image illustrates the necessary wire connections for the sensor package:


![Sensor Package Wiring](/images/sensor_package_wiring.png)


### Network setup <a name="networksetup"></a>

Below are the steps to initialize the server, gateway and one sensor node.

1. **MQTT client setup**

   Instructions on how to set up the MQTT client on the ESP8266 and ATMega will be written soon.


2. **MQTT Server setup.**

   Instructions on how to set up the MQTT server will be written soon.

### Uploading Sketches <a name="uploadsketches"></a>

As implied by the name of the board, this board allows one to upload sketches to two seperate microcontrollers - the ESP8266 and the ATMega328p (Arduino Uno) chips. The ESP8266 handles all WiFi communications and processes whereas the ATMega328p handles regular sensor processes as commonly done by the Arduino Uno. The board employs the CH340G USB-TTL converter in combination with dipswitches to allow one to configure how each chip's serial communications should operate.

![Dipswitches on board](/images/dipswitches.png)

#### Prequisites <a name="uploadprequisite"></a>
Before one can begin uploading sketches, they must first download a driver so that their computer may communicate with the CH340G. The documents tab on the [Robotdyn](https://robotdyn.com/uno-wifi-r3-atmega328p-esp8266-32mb-flash-usb-ttl-ch340g-micro-usb.html) website offers the driver download for free. Download the driver for your specific operating system, and follow the README that is included with your download.

#### Modes of Serial Operation <a name="serialmodes"></a>

The following table lists the available configurations for the board's serial communications, where switch 8 is not used:

| Mode of Serial Operation | SW1 | SW2 | SW3 | SW4 | SW5 | SW6 | SW7 | SW8 |
|--------------------------|-----|-----|-----|-----|-----|-----|-----|-----|
| Upload Sketch to ESP8266 | OFF | OFF | OFF | OFF | ON  | ON  | ON  | N/A | 
| Serial Port to ESP8266   | OFF | OFF | OFF | OFF | ON  | ON  | OFF | N/A |
| Upload Sketch to ATMega  | OFF | OFF | ON  | ON  | OFF | OFF | OFF | N/A |
| ESP8266 to ATMega        | ON  | ON  | OFF | OFF | OFF | OFF | OFF | N/A |
| All Chips Independent    | OFF | OFF | OFF | OFF | OFF | OFF | OFF | N/A |


1. Upload Sketch to ESP8266

   This mode is used to upload sketches to the ESP8266 chip. Do not use this mode to connect to a tethered computer's serial monitor.

2. Serial Port to ESP8266

   This mode is used to allow the ESP8266 to send data over its serial port via the USB cable. Common usage for this mode is to connect to the serial port on the ESP8266 via a serial monitor on a computer tethered by USB.

3. Upload Sketch to ATMega
   
   This mode is used to upload sketches to the ATMega chip. The ATMega can also send data to a tethered computer's serial monitor via USB in this mode.

4. ESP8266 to ATMega

   This mode is used to enable serial communications between the ESP8266 and the ATMega chip, without the need for external wirings. Thus, any serial prints from the ESP8266 will be available at the ATMega's serial port, and vice versa. This mode is the default mode used in production.

5. All Chips Independent

   This mode effectively cuts off all internal serial communications, including those done over a USB cable.

It is important to disconnect your board before configuring a different mode.

#### Uploading an ESP8266 Sketch <a name="uploadesp"></a>

First, flip the appropriate dip switches to set the serial communications mode to Upload Sketch to ESP8266. That would require switches 5, 6, and 7 to be flipped on with the remaining switches flipped off. After flipping the appropriate switches, connect the board to your computer.

Then, edit the platform.ini file to use the esp8266_on_uno environment.

```
[platformio]
#default_envs = uno
default_envs = esp8266_on_uno
```

Ensure that the upload_port points to the correct serial port. On Mac, you can enter
```
ls /dev/tty*
```
to find the serial port that your board is connected to.

Finally, click the upload button and wait until the sketch is uploaded before disconnecting your board. 

#### Uploading an ATMega Sketch <a name="uploadatmega"></a>

First, flip the appropriate dip switches to set the serial communications mode to Upload Sketch to ATMega. That would require switches 3 and 4 to be flipped on with the remaining switches to be flipped off. Then, connect the board to your computer.

Next, edit the platform.ini file to use the uno environment
```
[platformio]
default_envs = uno
#default_envs = esp8266_on_uno
```
Finally, click the upload button and wait until the sketch is uploaded before disconnecting your board.

### Running the Sensor Package <a name="runpackage"></a>

Before the sensor package can be ran, the sketches for the ESP8266 and the ATMega chips must be uploaded as described in the [Uploading Sketches](#uploadsketches) section. Once the sketches are uploaded, the board must be configured in the [ESP8266 to ATMega](#serialmodes) serial communication mode as each chip must communicate with each other.

Once the board is properly configured, the board will power on and begin operation. Note that you may need to press the MCU reset button each time the board is connected to a power source.

## Research Analysis <a name="ra"></a>
### Cost Scaling <a name="cost"></a>
The following table documents the cost of the core of the network - the Ethernet and gateway nodes.

| Component                                                                   | Price   |
|-----------------------------------------------------------------------------|---------|
| [Arduino Uno + Wifi](https://www.amazon.com/dp/B0834JQ5Y5?ref_=pe_1815430_211938580) | $11.99 |
| [Gravity: Analog Infrared CO2 Sensor](https://www.dfrobot.com/product-1549.html)    | $58.00  |
| [PM2.5 Air Quality Sensor](https://www.adafruit.com/product/3686)                    | $39.95   |
| [Si7021 Temperature & Humidity Sensor](https://www.adafruit.com/product/3251)        | $9.95   |
| [MicroSD Card Driver Module (5pcs)](https://www.amazon.com/Adapter-Reader-interface-driver-Arduino/dp/B01MSNX0TW/) | $9.28 |
| Total (exlcuding shipping)                                               | $129.17 |

## Releases <a name="releases"></a>
Not currently released.