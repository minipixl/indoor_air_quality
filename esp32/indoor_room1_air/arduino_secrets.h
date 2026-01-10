// common entries
#define SECRET_SSID "your ssid"
#define SECRET_PASS "your pw"

#define mqttserverIP "192.168.1.xxx" // IP4 of your Raspi

// I have 2 different esp32 and need to adapt internal I2C assignment. Remove // for your device.
// offsets: I measured wrong values and adjusted with offsets

// WROVER-IE KIT entries device2
#define I2C_SDA 6
#define I2C_SCL 7
// offsets if needed
#define temp_offset 0
#define hum_offset 0


// C6-WROOM-1 KIT entries device1
// #define I2C_SDA 21
// #define I2C_SCL 22
// offsets if needed
// #define temp_offset -0
// #define hum_offset 0