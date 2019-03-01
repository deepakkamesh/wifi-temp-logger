#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <BlynkSimpleEsp8266.h>

// Setup multiple log levels.
#ifdef DEBUG
#define DEBUG_PRINT(x)  Serial.println (x)
#else
#define DEBUG_PRINT(x)
#endif

#ifdef DEBUG_V2
#define DEBUG_PRINT_V2(x)  Serial.println (x)
#else
#define DEBUG_PRINT_V2(x)
#endif

#define DEBUG
//#define DEBUG_V2

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
int pushInterval = 5000; // milliseconds to wait between data push.

void setup()
{

  // Startup devices.
  delay(500);
#if defined(DEBUG) || defined(DEBUG_V2)
  Serial.begin(57600);
#endif
  dht.begin();
  DEBUG_PRINT("\nStarted...");
  EEPROM.begin(MAX_EEPROM_SZ);

  if (!readConfigEEPROM(blynkAuth)) {
    DEBUG_PRINT_V2("EEPROM config not found");
  }

  WiFiManager wifiManager;
  wifiManager.setDebugOutput(false);
  WiFiManagerParameter customBlynkAuth("blynk_auth", "blynk auth token", blynkAuth, MAX_BLYNK_TOKEN_SZ);
  WiFiManagerParameter customPushInt("push_interval", "Data push interval(ms)", "5000" , 4);
  wifiManager.addParameter(&customBlynkAuth);
  wifiManager.addParameter(&customPushInt);
  wifiManager.setSaveConfigCallback(saveConfigCallback);

#ifdef DEBUG_V2
  wifiManager.setDebugOutput(true);
#endif

  if (!wifiManager.autoConnect("chaneySensor", "password")) {
    DEBUG_PRINT("Failed to connect to Wifi network.");
    delay(5000);
    ESP.reset();
    delay(5000);
  }

  //Read updated parameters.
  strcpy(blynkAuth, customBlynkAuth.getValue());
  pushInterval = atoi(customPushInt.getValue());

  if (bSaveConfig) {
    writeConfigEEPROM(blynkAuth);
  }

  DEBUG_PRINT("Connected to WiFi. Local ip:");
  DEBUG_PRINT(WiFi.localIP());

  // Setup and connect to Blynk.
  delay(1000);
  Blynk.config(blynkAuth);
  if (!Blynk.connect()) {
    DEBUG_PRINT("Blynk Connection Fail");
    WiFi.disconnect(true);
    delay (2000);
    ESP.reset();
    delay (5000);
  }
  DEBUG_PRINT("Connected to Blynk.");
  timer.setInterval(pushInterval, sendSensor);
  EEPROM.end();
}

void loop() {
  Blynk.run();
  timer.run();
}


void serviceBlinker() {
  int state = digitalRead(LED_BUILTIN);  // get the current state of GPIO1 pin
  digitalWrite(LED_BUILTIN, !state);     // set pin to the opposite state
}

// readConfigEEPROM reads the auth from EEPROM.
bool readConfigEEPROM(char auth[]) {
  if (EEPROM.read(0) != 212) {
    DEBUG_PRINT_V2("EEPROM empty");
    return false;
  }
  byte sz = EEPROM.read(1);
  int i;
  for (i = 2; i <= sz + 1; i++) {
    auth[i - 2] = EEPROM.read(i);
  }
  auth[i - 2] = '\0';
  DEBUG_PRINT_V2("Read config from EEPROM:");
  DEBUG_PRINT_V2(auth);
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
    DEBUG_PRINT("Failed to commit");
  }
  DEBUG_PRINT_V2("Wrote config to EEPROM:");
  DEBUG_PRINT_V2(auth);
}

// Callback notifying us of the need to save config.
void saveConfigCallback () {
  DEBUG_PRINT_V2("Should save config");
  bSaveConfig = true;
}

// sendSensor send temp/humidity data to Blynk.
void sendSensor()
{
  // Verify if blynk is connected. If not reset ESP.
  if (!Blynk.connected()) {
    DEBUG_PRINT("Blynk disconnected. Attempting to connect");
    delay (1000);
    if (!Blynk.connect()) {
      DEBUG_PRINT("Blynk connection failed. Resetting..");
      ESP.reset();
      delay (2000);
    }
    DEBUG_PRINT("Connected to Blynk");
  }

  float h = dht.readHumidity();
  float t = dht.readTemperature(true); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    DEBUG_PRINT("Failed to read from DHT sensor!");
    return;
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(TEMP_VPIN, t);
  delay(500);
  Blynk.virtualWrite(HUMIDITY_VPIN, h);

  DEBUG_PRINT_V2("Temp:");
  DEBUG_PRINT_V2(t);
  DEBUG_PRINT_V2("  ");
  DEBUG_PRINT_V2("Humidity:");
  DEBUG_PRINT_V2( h);
  DEBUG_PRINT_V2("\n");
}
