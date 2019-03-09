# Chaney WiFi Temperature/Humidity Sensor

> **Follow the instructions to [program](https://github.com/deepakkamesh/wifi-temp-logger/blob/master/docs/Programming.md) the sensor first before following this guide**

## Features

 - The temperature/humidity sensor uses a DHT11 and ESP8266 to measure the readings and sends them via WiFi to a remote server. 
 - The readings can be accessed from a smart phone app from anywhere in the world.
 ![Screenshot](https://github.com/deepakkamesh/wifi-temp-logger/blob/master/docs/Screenshot.png)
 - You can also configure the app to send alerts to email if the temperature or humidity goes above or below thresholds. 

## Schematic
![enter image description here](https://github.com/deepakkamesh/wifi-temp-logger/blob/master/schematic.png?raw=true)

## Configuration of Sensor
### Setup Smart Phone App

 1. Download and install the Blynk app on your phone. This app is available for [Android](http://j.mp/blynk_Android) and [IOS](http://j.mp/blynk_iOS).
 2. Open the app and create an account (its free).
 3. After logging into the app, touch the QR-code icon and point the camera to the code below. This will setup the app configuration. 
 ![enter image description here](https://github.com/deepakkamesh/wifi-temp-logger/blob/master/docs/qrcode.png?raw=true)
 
4. Click on the project settings icon (on the top bar that looks like a nut) and scroll down till you see auth tokens. Click on email all. 
5. Check your email for the auth token. (eg. Auth Token : b7ddcea756bc4c239fba6c1e6ab1b3d8). Make a note of this number.

### Sensor Configuration
 1. Power up your sensor. The blue led should come on indicating that the sensor is ready to accept configuration.
 2. You will notice a new WiFi access point called ChaneySensor. Connect to this access point.
 3. Open a browser and type in this url http://192.168.4.1/
 4. Select *"Configure WiFi"*
 5. Your home wifi network should show up in the list. Select it and type in your network's password.
 6. Enter the blynk auth token from earlier into the *"blynk auth token"* field
 7. The field where is states 5000 is the update frequency given in milliseconds. This means the temperature and humidity data is send to servers every 5000 milliseconds. (5secs). Leave it as default or update if needed. 
 8. Hit the save button. 
 9. Now wait for about 30 seconds till the blue led turns off. This indicates the configuration was successful. If the blue led continues to be on after 30 seconds, power down and power up and try from step 1 again. 
 10. Open the blynk app and start your project by hitting the play icon (on the top bar that looks like a play button)
 11. If everything went well you should see data from your sensor. 
 12. You can customize the Blynk app. For advanced configurations see the [documentation](https://docs.blynk.cc)

 
