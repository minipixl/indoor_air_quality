# indoor_air_quality
ESP32 sending data via MQTT to Raspi4

## 'Why?'
For me this was a learning project. I wanted to connect 2 remote ESP32 with sensors to a central server running on a Raspberry Pi4.
Both ESP32 are connected to a sensor multi-function environmental sensor from DFRobot with ENS160 + BME280. I wanted to see it working over the whole chain from reading remote sensors to graphical time series presentation.

## 'What?"

The sensor offers a number of pyhsical quantities:
|Short Name | Description | Unit or Range |
|---|---|---|
|Ts |time stamp| yyyy-mm-dd hh:mm:ss |
|temp  | temperature | °C  |
|hum  | humidity | 5-95%RH |
|pres | Pressure | 300~1100hPa|
|alt | barometric pressure| m above sea level |
|aqi  | AQI reference| 1 .. 5 |
|tvoc |  TVOC – Total Volatile Organic Compounds| ppb |
|eco2 | CO2 | ppm |

**Mechanical / HW**\
The ESPs with sensor are located in different rooms, each powered by a USB charging device.\
Enclosure: I 'misused' a plastic box intended for cable connections. I see 3 main functions:
   * protect the device from damage, dust and touching
   * provide mounting feature
   * ensure environmental air contact to sensor (2 holes in enclosure) without heat disturbance from ESP devkit (separation plate).

![enclosure1](https://github.com/user-attachments/assets/416c4fbb-ca7f-453b-97a0-8eea6a342c5c)

For the RasPi I use half of the enclosure because of better heat dissipation. Pending for improvments :-)\
Placement is 'somewhere', important is WLAN connection. And make sure to use a 'real' Raspi power supply to avoid strange behavior with no chance for reproducable fault tracing..

**Connections and local buffering**\
ESP32s and Raspi are connected to the same WLAN.\
Measurement is sampled every 60s, then transferred via MQTT to the Raspi. \
I can access the Raspi via ssh or and FileZilla (port 22), to InfluxDB (port 8086) or to Grafana (port 3000).\
Port fowarding on your router allows access to the Raspi from outside of your private network.\

WLAN and other stuff is switched off at night, so the ESPs cannot transmit data to Raspi. Without connection, the ESPs are buffering all measurements with timestamp. After WLAN is switched on, ESPs and Raspi reconnect to WLAN. ESPs reconnect to MQTT broker, then transmitt all buffered data and empty the buffer. Protection from overflow still pending..\





The sensor offers an I2C interface which makes it easy to connect to the ESP32 devkit by 4 wires (5V, GND, SDA, SCL). \
Make sure to find the right pins, distinguish GPIO numbering from header pins. In my case:\
WROVER-IE KIT: SDA= 6, SCL= 7\
C6-WROOM-1 KIT: SDA= 21, SCL= 22\
Sketch is for Arduino IDE2, same for both kits. Adjust the .secrets file.h with ssid, pw and the pin assignment.


<img width="2844" height="1780" alt="image" src="https://github.com/user-attachments/assets/3df02915-7671-4b42-a1dc-e3e1b00c07ff" />
