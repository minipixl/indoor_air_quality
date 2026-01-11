# indoor air quality logging
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

## 'How?'

See the architecture.\ 
Signal flow is from the sensors to the ESP32 via I2C.\
Then transmission to the Raspi via MQTT using local WLAN. ESP32 uses Pusbuclient.\
The Raspi is a model 4 with 4GB. I installed Ubuntu server LTS 22.04.
Inside the Raspi Mosquitto is the MQTT broker, then Telegraf is a bridge to InfluxDB, which is the database storing all measurements.\
Grafan is used to visualize data and for downsampling of data. Currently downsampling of buffered data is not working yet - due to the downsampling method chosen.\
The database and configurations are backed each night on a USB-stick. Backups on the same SD card would be useless if this SD fails. The stick will be mounted before the backup and unmounted afterwards. This is controlled by cron.\

<img width="2844" height="1780" alt="image" src="https://github.com/user-attachments/assets/3df02915-7671-4b42-a1dc-e3e1b00c07ff" />

**Scalability**\
The number of ESPs can easily be increased, just adapt the MQTT hierarchical names.
The number of signals can be extended, some might need different sampling rates which needs structural modification in the sketch since the main loop is focusing on 60s sensor sampling.


<img width="4292" height="2960" alt="image" src="https://github.com/user-attachments/assets/fc0de48e-e73f-4123-9998-4eb29887af0f" />
