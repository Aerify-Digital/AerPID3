#ifndef LIGHTSSTOR_H
#define LIGHTSSTOR_H

#include "flash.h"
#include "SPIFFS.h"


class LightsStor
{
public:
  void init(Flash *flash);

  void load();
  void save();

  bool setup();

  bool isEnabled();
  void isEnabled(bool state);

  bool statusEnabled();
  void statusEnabled(bool state);

  bool ambientEnabled();
  void ambientEnabled(bool state);

  bool amPulseEnabled();
  void amPulseEnabled(bool state);

  bool amBlinkEnabled();
  void amBlinkEnabled(bool state);

  bool presetsEnabled();
  void presetsEnabled(bool state);

  bool preRainEnabled();
  void preRainEnabled(bool state);

  bool preRain2Enabled();
  void preRain2Enabled(bool state);

  bool preRain3Enabled();
  void preRain3Enabled(bool state);

  bool prePulseEnabled();
  void prePulseEnabled(bool state);

  bool preShiftEnabled();
  void preShiftEnabled(bool state);

  uint8_t getRgbVal(char color);
  //unsigned long getRgbHex();
  String getRgbHex();
  String decToHex(int n);
  void setRgbVal(char color, int8_t val);

  uint8_t getBright();
  void setBright(int8_t bright);

  uint8_t getMode();
  void setMode(int8_t mode);

private:
  bool loaded = false;
  Flash *flash;

  uint8_t LED_R = 255;
  uint8_t LED_G = 0;
  uint8_t LED_B = 255;
  uint8_t LED_Bright = 255;

  bool enabled = true;
  bool led_status = true;
  bool led_ambient = false;
  bool led_am_pulse = false;
  bool led_am_blink = false;
  bool led_preset = false;
  bool led_pre_rain = false;
  bool led_pre_rain2 = false;
  bool led_pre_rain3 = false;
  bool led_pre_pulse = false;
  bool led_pre_shift = false;
};

extern LightsStor lightstor;

#endif