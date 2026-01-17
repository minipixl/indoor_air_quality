## Indoor air quality measurement and logging

# Sensor
I used a "Fermion: ENS160+BME280 Environmental Sensor - TVOC, eCO2, Temp & Humidity (Breakout)"
https://www.dfrobot.com/product-2064.html
The sensor offers an I2C interface which makes it easy to connect to the ESP32 devkit by 4 wires (3.3V, GND, SDA, SCL). \

I observed high temperature offsets of +6 to +8 Kelvin, despite my attempts to isolate sensor from ESP board and natural ventilation in the enclosure.\
I solved this by adjusting an offset temperature determined with a reference measurement.\
The offset is much higher than the accuracy stated in the data sheet. I also didn't figure out how the preheating of the ENS160 affects the tempereture sensor on BME280.

# ESP32
The ESP32 are not limited to devkits, it should be possible to use other types as well. Just make sure you have WLAN and I2C pins available.
Make sure to find the right pins, distinguish GPIO numbering from header pins. In my case:\
WROVER-IE KIT: SDA= 6, SCL= 7\
C6-WROOM-1 KIT: SDA= 21, SCL= 22\
The sketch is for Arduino IDE2, same for both kits. Adjust the .secrets file.h with ssid, pw and the I2C pin assignment.
Power supply is a standard USB charger, always connected.

# Raspberry Pi4 Model B 4GB
First I tried PI OS lite and had problems with WLAN, 2.4 GHz was not possible. I switched to Ubuntu server LTS 22.04 and it worked.






