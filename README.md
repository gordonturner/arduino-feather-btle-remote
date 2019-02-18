Arduino-BTLE-Remote
===================

This is a sketch for the Adafruit Feather 32u4 Bluefruit LE:

[https://www.adafruit.com/product/2829](https://www.adafruit.com/product/2829)

The pinout for the board is here:

https://learn.adafruit.com/assets/46242

This is based off the example sketch `hidcontrolkey` from Adafruit.

Specifically it uses the following key commands:

```
PLAYPAUSE
MEDIAPREVIOUS
MEDIANEXT
MUTE
VOLUME+
VOLUME-
```

Board Discovery
---------------

Need to connect 6 buttons to the 







Setup and Configure Feather Support
-----------------------------------

- Reference:

https://learn.adafruit.com/adafruit-feather-32u4-bluefruit-le/installing-ble-library

- Download `Adafruit_BluefruitLE_nRF51-master` from:

https://github.com/adafruit/Adafruit_BluefruitLE_nRF51/archive/master.zip

- Copy to `~/Documents/Arduino/libraries` and uncompress

```
cp ~/Downloads/Adafruit_BluefruitLE_nRF51-master.zip ~/Documents/Arduino/libraries
cd ~/Documents/Arduino/libraries
unzip Adafruit_BluefruitLE_nRF51-master.zip
```

- Restart Arduino IDE
- After restarting, click File, Examples, Adafruit BluefruitLE nRf51


Setup Feather Boards
--------------------

- Start Arduino IDE
- Click Arduino, Preferences
- In `Additional Boards Manager URLs` add:
https://adafruit.github.io/arduino-board-index/package_adafruit_index.json

- Restart Arduino IDE
- Click Tools, Board, Boards Manager and search for:
Adafruit AVR Boards

- Click Add

- Reference:

https://learn.adafruit.com/adafruit-feather-32u4-basic-proto/using-with-arduino-ide#


Uploading Sketches
------------------

- Connect board to laptop
- Confirm port appears
- Click upload and then click the reset button on the board





