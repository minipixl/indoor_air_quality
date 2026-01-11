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





The sensor offers an I2C interface which makes it easy to connect to the ESP32 devkit by 4 wires (5V, GND, SDA, SCL). 
Make sure to find the right pins, distinguish GPIO numbering from header pins. In my case:
WROVER-IE KIT: SDA= 6, SCL= 7
C6-WROOM-1 KIT: SDA= 21, SCL= 22
Sketch is for Arduino IDE2, same for both kits.



<img width="2844" height="1780" alt="image" src="https://github.com/user-attachments/assets/3df02915-7671-4b42-a1dc-e3e1b00c07ff" />
