#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <BlynkSimpleEsp8266.h>

#define MAX_BLYNK_TOKEN_SZ 34
#define MAX_EEPROM_SZ 100
#define TEMP_VPIN V1
#define HUMIDITY_VPIN V2

// Setup DHT11 params.
#define DHTPIN 2 // GPIO pin connected to DHT11.
#define DHTTYPE DHT11 // Types: DHT11, DHT22.
DHT dht(DHTPIN, DHTTYPE);

// Setup Blynk.
BlynkTimer timer;
char blynkAuth[] = "";

// Globals.
bool bSaveConfig = false; // If set config must be saved.


void setup()
{
  delay(1000);
  Serial.begin(115200);
  dht.begin();
  Serial.println("\nStarted...");

  EEPROM.begin(MAX_EEPROM_SZ);

  if (!readConfigEEPROM(blynkAuth)) {
    Serial.println("EEPROM config not found");
  }

  WiFiManagerParameter customBlynkAuth("blynk", "blynk auth token", blynkAuth, MAX_BLYNK_TOKEN_SZ);
  WiFiManager wifiManager;
  wifiManager.addParameter(&customBlynkAuth);
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  if (!wifiManager.autoConnect("chaneySensor", "password")) {
    Serial.println("Failed to connect to Wifi network.");
    delay(3000);
    ESP.reset();
    delay(5000);
  }

  //Read updated parameters.
  strcpy(blynkAuth, customBlynkAuth.getValue());

  if (bSaveConfig) {
    writeConfigEEPROM(blynkAuth);
  }

  Serial.print("Connected to WiFi. Local ip:");
  Serial.println(WiFi.localIP());

  // Setup and connect to Blynk.
  delay(1000);
  Blynk.config(blynkAuth);
  if (!Blynk.connect()) {
    Serial.println("Blynk Connection Fail");
    WiFi.disconnect(true);
    delay (2000);
    ESP.reset();
    delay (5000);
  }
  Serial.println("Connected to Blynk.");
  timer.setInterval(2500L, sendSensor);
  EEPROM.end();
}

void loop() {
  Blynk.run();
  timer.run();
}

// readConfigEEPROM reads the auth from EEPROM.
bool readConfigEEPROM(char auth[]) {
  if (EEPROM.read(0) != 212) {
    Serial.println("EEPROM empty");
    return false;
  }
  byte sz = EEPROM.read(1);
  int i;
  for (i = 2; i <= sz + 1; i++) {
    auth[i - 2] = EEPROM.read(i);
  }
  auth[i - 2] = '\0';
  Serial.print("Read config from EEPROM:");
  Serial.println(auth);
}

// writeConfigEEPROM writes the auth from EEPROM. sz should be < 256.
void writeConfigEEPROM(char auth[]) {
  // Write EEPROM header '212'.
  EEPROM.write(0, 212);

  // Write size at first byte.
  byte sz = strlen(auth);
  EEPROM.write(1, sz );

  for (byte i = 2; i <= sz + 1; i++) {
    EEPROM.write(i, auth[i - 2]);
  }
  if (!EEPROM.commit()) {
    Serial.println("Failed to commit");
  }
  Serial.print("Wrote config to EEPROM:");
  Serial.println(auth);
}

// Callback notifying us of the need to save config.
void saveConfigCallback () {
  Serial.println("Should save config");
  bSaveConfig = true;
}

// sendSensor send temp/humidity data to Blynk.
void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature(true); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(TEMP_VPIN, t);
  Blynk.virtualWrite(HUMIDITY_VPIN, h);
  Serial.print("Temp:");
  Serial.print(t);
  Serial.print("  ");
  Serial.print("Humidity:");
  Serial.print( h);
  Serial.print("\n");
}
