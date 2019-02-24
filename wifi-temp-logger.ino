#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <BlynkSimpleEsp8266.h>

#define MAX_BLYNK_TOKEN_SZ 34
#define MAX_EEPROM_SZ 100

// Setup DHT11 params.
#define DHTPIN 2 // GPIO pin connected to DHT11.
#define DHTTYPE DHT11 // Types: DHT11, DHT22.
DHT dht(DHTPIN, DHTTYPE);

// Setup Blynk.
BlynkTimer timer;
char blynkAuth[] = "";

// Globals.
bool bSaveConfig = false; // If set config must be saved.

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
  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V6, t);
  Serial.print("Temp:");
  Serial.print(t);
  Serial.print("  ");
  Serial.print("Humidity:");
  Serial.print( h);
  Serial.print("\n");
}


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

  // Manual connection.
  /* WiFi.begin("utopia", "0d9f48a148");
    while (WiFi.status() != WL_CONNECTED)
    {
     delay(500);
     Serial.print(".");
    }*/

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
  Serial.println("Blynk Connected.");
  timer.setInterval(2000L, sendSensor);
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

// readConfig reads config from disk. Returns true if read
// was successful.
bool readConfigFromDisk(char auth[]) {
  if (!SPIFFS.exists("/config.json")) {
    Serial.println("Config file ./config.json not found.");
    return false;
  }
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open ./config.");
    return false;
  }
  // Read from config file.
  size_t size = configFile.size();
  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  configFile.readBytes(buf.get(), size);
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());
  json.printTo(Serial);
  if (!json.success()) {
    Serial.println("Failed to parse json.");
    return false;
  }
  Serial.println("Config loaded from disk.");
  strcpy(auth, json["blynk_auth"]);
  configFile.close();
}

// saveConfigToDisk saves config to disk.
void saveConfigToDisk()  {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["blynk_auth"] = blynkAuth;

  File configFile = SPIFFS.open("/config", "w+");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return ;
  }
  json.printTo(Serial);
  json.printTo(configFile);
  configFile.close();
  Serial.println("\nSaved config to disk.");
}


// checkFileSystem verifies if FS is ready, if not formats.
bool checkFileSystem()  {
  if (SPIFFS.exists("/formatComplete")) {
    Serial.println("SPIFFS is formatted. Moving along...");
    return true;
  }

  Serial.println("Please wait 30 secs for SPIFFS to be formatted");
  if (!SPIFFS.format()) {
    Serial.println("Format failed.");
    return false;
  }
  Serial.println("Filesystem formatted");

  File f = SPIFFS.open("/formatComplete", "w");
  if (!f) {
    Serial.println("Format complete, but unable to create formatComplete file");
    return false;
  }
  return true;
}
