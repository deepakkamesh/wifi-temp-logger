# Programming the Chaney WiFi Sensor

## Programmer Setup (one time)
### Parts Needed
 1. USB to serial TTL  UART module ([eBay](https://www.ebay.com/itm/1X-USB-2-0-to-TTL-RS232-TTL-UART-Module-Serial-Converter-Adapter-CH340G-MudC2C8/273570810247?ssPageName=STRK:MEBIDX:IT&_trksid=p2060353.m2749.l2649))
 2. ESP8266 generic board (ESP-01) ([eBay](https://www.ebay.com/itm/2X-ESP8266-Serial-WIFI-Wireless-Transceiver-Module-Send-Receive-AP-STA-ESP-01-US/382619187384?epid=1452674900&hash=item5915e178b8:g:aXoAAOSwI61b35QB&frcectupt=true))

### Software Setup
 1. Download the Serial to USB adapter driver depending on the chipset of the programmer. The easiest thing is to Google the IC number on the serial to USB adapter and download the drivers for your operating system.
 2. Download and install the [Arduino IDE](https://www.arduino.cc/en/Main/Software) 
 3. Setup Arduino IDE for programming ESP8266
      A. Open Arduino IDE
      B. click on File -> Preferences".
      C. In “Additional Boards Manager URLs” add this line and click on “OK”: 		   "http://arduino.esp8266.com/stable/package_esp8266com_index.json”
      D. Go to “Tools -> Board -> Boards Manager”, type “ESP8266” and install it.
 4. Download the compiled binary for the Sensor kit. [[link to compiled binary]](https://github.com/deepakkamesh/wifi-temp-logger/raw/master/wifi-temp-logger.ino.generic.bin) to a directory.
 5. Find the location of  the *esptool* on your computer use file search. Make a note of this location as it would be used during programming. (For eg: c:/Appdata/deepak/Arduino15/packages/esp8266/tools/esptool/2.5.0-3-20ed2b9/esptool)
 6. Plug in the serial to us programmer into the USB port and go to device manager to determine the serial port (For eg. COM5 on windows)

## Programming the ESP8266
### Connection

![Programmer connection](https://raw.githubusercontent.com/deepakkamesh/wifi-temp-logger/master/docs/esp8266%20programmer.jpeg)

### Programming

 1. After making the connections as shown above, plug into USB port. 
 2. Open an command prompt (depends on your OS) and go to the directory where you downloaded the binary from step #4 of software setup.
 3. Run the command from the command prompt after substituting the right values for your system
> **<Path to esptool from step 5 of software setup>**/esptool -vv -cd ck -cb 115200 -cp **\<Serial port number from step 6\>** -ca 0x0 -cz 0x100000 -ca 0x00000 -cf wifi-temp-logger.ino.bin

For Example on windows:

> c:/Users/AppData/dkg/Library/Arduino15/packages/esp8266/tools/esptool/2.5.0/esptool -vv -cd ck -cb 115200 -cp COM5 -ca 0x0 -cz 0x100000 -ca 0x00000 -cf wifi-temp-logger.ino.bin

### Verification
1. Once the programming is complete the blue LED on the esp board will light up solid indicating the board is ready for configuration.
2. Additionally when you scan your wireless network from phone or laptop you will notice a new WiFi point called "chaneySensor".
