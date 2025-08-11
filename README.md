# AerPID3 - Control software for the AerTiny Temperature Controller

Dedicated [ESP32](https://www.espressif.com/en/products/modules/esp32) Based PID Controller for a high-power Heating Element. Allows stable temperature management of a heating element.

For Purchase: https://aerify.digital/product/aertiny3-2/

## Features

- ESP32-Wroom with 16 MB flash storage
- PID control of SSR (AC)
- Wifi enabled with web interface
- LCD Screen with 320x240 pixel resolution
- Rotary Encoder with two Button interface
- 8 kilobyte EEPROM storage
- 16 MB Flash storage
- Fancy LEDs (addressable)
- Onboard (2) Temperature sensors

## Setup

- VSCode - https://code.visualstudio.com/
- Platform.io - https://platformio.org/
- Clone project into a directory
- Initialize Project submodules:
  - `git submodule update --init --recursive`
- Build and upload using `Platform.io`

## Libraries Used

- TFT_eSPI - https://github.com/Bodmer/TFT_eSPI
- SerialFlash - https://github.com/Aerify-Digital/SerialFlash
- FastLED - https://github.com/FastLED/FastLED
- Adafruit_GFX - https://github.com/adafruit/Adafruit-GFX-Library
- Adafruit_NeoPixel - https://github.com/adafruit/Adafruit_NeoPixel
- AsyncTCP - https://github.com/ESP32Async/AsyncTCP
- ESPAsyncWebServer - https://github.com/ESP32Async/ESPAsyncWebServer
- extEEPROM - https://github.com/JChristensen/extEEPROM
- LM75 - https://github.com/thefekete/LM75
