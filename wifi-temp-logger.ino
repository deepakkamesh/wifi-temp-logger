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
char blynkAuth[] = "e88190aa80ca4732b95b9e1af033496e";

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

  /*  EEPROM.begin(MAX_EEPROM_SZ);

    if (!readConfigEEPROM(blynkAuth)) {
      Serial.println("EEPROM config not found");
    }
  */

  WiFiManagerParameter customBlynkAuth("blynk", "blynk auth token", "", MAX_BLYNK_TOKEN_SZ);
  WiFiManager wifiManager;
  wifiManager.addParameter(&customBlynkAuth);
//  wifiManager.setSaveConfigCallback(saveConfigCallback);

  if (!wifiManager.autoConnect("chaneySensor", "password")) {
    Serial.println("Failed to connect to Wifi network.");
    delay(3000);
    ESP.reset();
    delay(5000);
  }

  //Read updated parameters.
  strcpy(blynkAuth, customBlynkAuth.getValue());

  /*
    if (bSaveConfig) {
    writeConfigEEPROM(blynkAuth);
    }
  */

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
}

void loop() {
  Blynk.run();
  timer.run();
}
