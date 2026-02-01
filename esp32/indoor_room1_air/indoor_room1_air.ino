#include <WiFi.h>
#include "arduino_secrets.h"
#include "configs.h"
#include <PubSubClient.h>
#include <Wire.h>
#include <LittleFS.h>
#include <time.h>
#include <ArduinoJson.h>

#include <DFRobot_ENS160.h>
#include "DFRobot_BME280.h"

// ================== I2C ==================
// #define I2C_SDA 21 siehe ardu...h
// #define I2C_SCL 22
#define SEA_LEVEL_PRESSURE 1015.0f

DFRobot_ENS160_I2C ENS160(&Wire, 0x53);
typedef DFRobot_BME280_IIC BME;
BME bme(&Wire, 0x76);

// ================== WLAN ==================
const char* ssid     = SECRET_SSID;
const char* password = SECRET_PASS;

// ================== MQTT ==================
const char* mqttServer = mqttserverIP;
const int   mqttPort    = 1883;
const char* mqttTopic   = RAUM;
const char* statusTopic = STATS;

WiFiClient espClient;
PubSubClient mqtt(espClient);

// ================== Timing ==================
const unsigned long measureInterval = 60000;
unsigned long lastMeasure = 0;
time_t lastMeasurementTs = 0;

// ================== Files ==================
const char* bufferFile = "/buffer.jsonl";

// ================== Hilfsfunktionen ==================
void printLocalTimeNow() {
  struct tm t;
  if (!getLocalTime(&t)) {
    Serial.print("Zeit n/a");
    return;
  }
  char buf[30];
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &t);
  Serial.print(buf);
}

void printLocalTimeFromUTC(time_t ts) {
  struct tm t;
  localtime_r(&ts, &t);
  char buf[30];
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &t);
  Serial.print(buf);
}


// ================== WLAN ==================
void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.print("[WIFI] Verbinde...");
  WiFi.begin(ssid, password);
  unsigned long start = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("\n[WIFI] Verbunden, IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n[WIFI] NICHT verbunden");
  }
}

// ================== NTP ==================
bool syncTime() {
  configTime(0, 0, "pool.ntp.org");

  struct tm t;
  if (getLocalTime(&t, 10000)) {
    Serial.println(" OK");
    return true;
  } else {
    Serial.println(" FEHLER");
    return false;
  }
}

// ================== ping test ==================
void testMQTTReachability() {
  Serial.print("[TEST] Teste Erreichbarkeit von ");
  Serial.print(mqttServer);
  Serial.print("...");
  
  if (espClient.connect(mqttServer, mqttPort)) {
    Serial.println(" erreichbar!");
    espClient.stop();
  } else {
    Serial.println(" NICHT erreichbar!");
  }
}


// ================== MQTT ==================
void connectMQTT() {
  if (mqtt.connected()) return;

  Serial.print("[MQTT] Verbinde zum Broker...");
  mqtt.setServer(mqttServer, mqttPort);

  String cid = DEVICE;

  if (mqtt.connect(cid.c_str(), statusTopic, 1, true, "offline")) {
    Serial.println(" verbunden");
    mqtt.publish(statusTopic, "online", true);
  } else {
    Serial.print(" FEHLER, rc=");
    Serial.println(mqtt.state());
  }
}

// ================== Buffer ==================
size_t bufferedCount() {
  if (!LittleFS.exists(bufferFile)) return 0;
  File f = LittleFS.open(bufferFile, FILE_READ);
  size_t count = 0;
  while (f.available()) {
    if (f.read() == '\n') count++;
  }
  f.close();
  return count;
}

void storeLocally(const String& line) {
  File f = LittleFS.open(bufferFile, FILE_APPEND);
  f.println(line);
  f.close();
}

void flushBuffer() {
  if (!mqtt.connected() || !LittleFS.exists(bufferFile)) return;

  Serial.println("[BUFFER] Sende gepufferte Daten... ");

  File f = LittleFS.open(bufferFile, FILE_READ);
  File tmp = LittleFS.open("/tmp.jsonl", FILE_WRITE);

  size_t sent = 0;
  while (f.available()) {
    String line = f.readStringUntil('\n');
    if (mqtt.publish(mqttTopic, line.c_str(), true)) {
      sent++;
      Serial.println(line); // monitor sent data
    } else {
      tmp.println(line);
    }
  }
  f.close();
  tmp.close();

  LittleFS.remove(bufferFile);
  LittleFS.rename("/tmp.jsonl", bufferFile);

  Serial.print("gesendet: ");
  Serial.print(sent);
  Serial.print(", verbleibend: ");
  Serial.println(bufferedCount());
}

// ================== Setup ==================
void setup() {
  Serial.begin(115200);
  delay(500);

  setenv("TZ", "CET-1CEST,M3.5.0/02,M10.5.0/03", 1);
  tzset();

  Wire.begin(I2C_SDA, I2C_SCL);

  if (!LittleFS.begin(true)) {
    Serial.println("[FS] LittleFS Fehler");
  }

  connectWiFi();
  
  Serial.print("[NTP] Zeit synchronisieren...");
  syncTime();

  testMQTTReachability();  // <-- NEU

  // BME280
  Serial.print("[BME280] Initialisiere...");
  bme.reset();
  while (bme.begin() != BME::eStatusOK) {
    Serial.print(".");
    delay(2000);
  }
  Serial.println(" OK");

  // ENS160
  Serial.print("[ENS160] Initialisiere...");
  while (ENS160.begin() != NO_ERR) {
    Serial.print(".");
    delay(3000);
  }
  ENS160.setPWRMode(ENS160_STANDARD_MODE);
  Serial.println(" OK");
}

// ================== Loop ==================
void loop() {
  connectWiFi();
  connectMQTT();
  mqtt.loop();

  if (millis() - lastMeasure >= measureInterval) {
    lastMeasure = millis();

    float temp = bme.getTemperature();
    float hum  = bme.getHumidity();
    uint32_t pres = bme.getPressure();
    float alt = bme.calAltitude(SEA_LEVEL_PRESSURE, pres);

    ENS160.setTempAndHum(temp, hum);
    uint8_t aqi = ENS160.getAQI();
    uint16_t tvoc = ENS160.getTVOC();
    uint16_t eco2 = ENS160.getECO2();

    time(&lastMeasurementTs);

    // correct offsets (measured)
    temp = temp + temp_offset;
    hum = hum + hum_offset;

    StaticJsonDocument<256> doc;
    doc["ts"]   = lastMeasurementTs;
    doc["temp"] = temp;
    doc["hum"]  = hum;
    doc["pres"] = pres;
    doc["alt"]  = alt;
    doc["aqi"]  = aqi;
    doc["tvoc"] = tvoc;
    doc["eco2"] = eco2;

    String payload;
    serializeJson(doc, payload);

    Serial.println("\n[MESSUNG]");
    Serial.print(" Zeit: ");
    printLocalTimeNow();                // aktuelle lokale Zeit (MEZ/MESZ)
    Serial.print(" (");
    printLocalTimeFromUTC(lastMeasurementTs); // lokale Zeit aus UTC
    Serial.print(" | UTC ");
    Serial.print(lastMeasurementTs);    // UTC Timestamp
    Serial.println(")");

    Serial.print(" Temp: "); Serial.print(temp); Serial.print(" °C");
    Serial.print(" | Hum: "); Serial.print(hum); Serial.print(" %");
    Serial.print(" | AQI: "); Serial.print(aqi);
    Serial.print(" | TVOC: "); Serial.print(tvoc);
    Serial.print(" | eCO2: "); Serial.println(eco2);

    if (mqtt.connected()) {
      flushBuffer();
      mqtt.publish(mqttTopic, payload.c_str(), true);
      Serial.println("[MQTT] Messung gesendet");
    } else {
      storeLocally(payload);
      Serial.println("[MQTT] Offline → lokal gespeichert");
    }

    Serial.print("[BUFFER] Noch nicht übertragen: ");
    Serial.println(bufferedCount());
  }
}
