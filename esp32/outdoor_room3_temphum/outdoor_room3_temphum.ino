#include <WiFi.h>
#include "arduino_secrets.h"
#include "configs.h"
#include <PubSubClient.h>
#include <Wire.h>
#include <LittleFS.h>
#include <time.h>
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// ================== Sensor ==================
Adafruit_BME280 bme;

#define SEA_LEVEL_HPA 1013.25f   // Lokalen Luftdruck ggf. anpassen

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

// ================== Ping-Test ==================
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

  Serial.print("[BUFFER] Sende gepufferte Daten... ");

  File f   = LittleFS.open(bufferFile, FILE_READ);
  File tmp = LittleFS.open("/tmp.jsonl", FILE_WRITE);

  size_t sent = 0;
  while (f.available()) {
    String line = f.readStringUntil('\n');
    if (mqtt.publish(mqttTopic, line.c_str(), true)) {
      Serial.println(line);
      sent++;
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

// ================== BME280 Init ==================
bool initBME280() {
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(400000);

  for (int attempt = 1; attempt <= 3; attempt++) {
    Serial.printf("  Versuch %d/3 ... ", attempt);
    if (bme.begin(BME280_ADDR, &Wire)) {
      bme.setSampling(
        Adafruit_BME280::MODE_NORMAL,
        Adafruit_BME280::SAMPLING_X2,
        Adafruit_BME280::SAMPLING_X16,
        Adafruit_BME280::SAMPLING_X1,
        Adafruit_BME280::FILTER_X16,
        Adafruit_BME280::STANDBY_MS_500
      );
      Serial.println("OK");
      return true;
    }
    Serial.println("fehlgeschlagen");
    delay(500);
  }
  return false;
}

// ================== Setup ==================
void setup() {
  Serial.begin(115200);
  delay(500);

  setenv("TZ", "CET-1CEST,M3.5.0/02,M10.5.0/03", 1);
  tzset();

  if (!LittleFS.begin(true)) {
    Serial.println("[FS] LittleFS Fehler");
  }

  connectWiFi();

  Serial.print("[NTP] Zeit synchronisieren...");
  syncTime();

  testMQTTReachability();

  Serial.print("[BME280] Initialisiere...");
  if (!initBME280()) {
    Serial.println("[BME280] FEHLER: Sensor nicht gefunden – prüfe Verkabelung!");
    while (true) { delay(5000); }
  }
}

// ================== Loop ==================
void loop() {
  connectWiFi();
  connectMQTT();
  mqtt.loop();

  if (millis() - lastMeasure >= measureInterval) {
    lastMeasure = millis();

    float temp = bme.readTemperature();
    float hum  = bme.readHumidity();
    float pres = bme.readPressure(); // 100.0f;   Pa → hPa keine runterskalierung
    float alt  = bme.readAltitude(SEA_LEVEL_HPA);

    if (isnan(temp) || isnan(hum) || isnan(pres)) {
      Serial.println("[BME280] Ungültige Messwerte – überspringe Zyklus");
      return;
    }

    time(&lastMeasurementTs);

    // Kalibrierungsoffsets anwenden
    temp = temp + temp_offset;
    hum  = hum  + hum_offset;

    StaticJsonDocument<192> doc;
    doc["ts"]   = lastMeasurementTs;
    doc["temp"] = serialized(String(temp, 2));
    doc["hum"]  = serialized(String(hum,  2));
    doc["pres"] = serialized(String(pres, 2));
    doc["alt"]  = serialized(String(alt,  1));

    String payload;
    serializeJson(doc, payload);

    Serial.println("\n[MESSUNG]");
    Serial.print(" Zeit: ");
    printLocalTimeNow();
    Serial.print(" (UTC ");
    Serial.print(lastMeasurementTs);
    Serial.println(")");
    Serial.print(" Temp: "); Serial.print(temp); Serial.print(" °C");
    Serial.print(" | Hum: ");  Serial.print(hum);  Serial.print(" %");
    Serial.print(" | Pres: "); Serial.print(pres); Serial.print(" hPa");
    Serial.print(" | Alt: ");  Serial.print(alt);  Serial.println(" m");

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