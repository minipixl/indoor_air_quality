# Indoor air quality measurement and logging
Sketch is the same for all indoor ESP32.\
Make sure to find the right board in board manager.\
Adapt the arduino_secrets.h file for each ESP32 board.

Outdoor has different ESP32 C6 Supermini and BME280 sensor (no: CO2, AQI, TVOC)

Outdoor wiring scheme:
Sensor 
SDO to GND
CSB to VCC (3V3)
SDA to GPIO6
SCL to GPIO7
GND to GND
VCC to 3V3 (not 5V)









