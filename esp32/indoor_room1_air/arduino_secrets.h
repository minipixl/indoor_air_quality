// common entries
#define SECRET_SSID "your ssid"
#define SECRET_PASS "your pw"

#define mqttserverIP "192.168.1.xxx" // IP4 of your Raspi

// I have 2 different esp32 and need to adapt internal I2C assignment. Remove // for your device.
// offsets: I measured wrong values and adjusted with offsets

// WROVER KIT entries Device1
#define I2C_SDA 21
#define I2C_SCL 22
// offsets seit 04.01.2025
#define temp_offset -6
#define hum_offset 8
#define RAUM "home/room1/air";
#define STATS "home/room1/status";
#define DEVICE "esp32-room1"


// WROVER KIT entries Device2
// #define I2C_SDA 6
// #define I2C_SCL 7
// offsets seit 04.01.2025
// #define temp_offset -6.5
// #define hum_offset 7
// #define RAUM "home/room2/air";
// #define STATS "home/room2/status";
// #define DEVICE "esp32-room2"