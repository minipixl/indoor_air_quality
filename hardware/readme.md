## Indoor air quality measurement and logging

The sensor offers an I2C interface which makes it easy to connect to the ESP32 devkit by 4 wires (5V, GND, SDA, SCL). \
Make sure to find the right pins, distinguish GPIO numbering from header pins. In my case:\
WROVER-IE KIT: SDA= 6, SCL= 7\
C6-WROOM-1 KIT: SDA= 21, SCL= 22\
Sketch is for Arduino IDE2, same for both kits. Adjust the .secrets file.h with ssid, pw and the pin assignment.






