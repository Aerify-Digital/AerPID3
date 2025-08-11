#ifndef BUTTONS_H
#define BUTTONS_H

#include "oled.h"

bool USE_CEL = false;

class Display
{

public:
  void init(oLED *led);

  void setPID(double p, double i, double d);

  void display_blank(int i, int j);
  void display_blank(int i, int j, int a);
  void display_main(bool USE_CEL, bool pidOn, bool EN_BUMP, double TEMP_M, double TEMP_S);
  void display_menu_pid(uint8_t i);
  void display_menu_pid_kP();
  void display_menu_pid_kI();
  void display_menu_pid_kD();
  void display_menu_pid_cursor(uint16_t s);
  void display_menu_con();
  void display_menu_con_go();
  void display_con_h();
  void display_con();
  void display_blu_connect_okay();
  void display_blu_connect_fail();
  void display_temp(int i);
  void display_set_temp(double t);
  void display_mes_temp(double t);
  void display_menu_home(uint8_t l);
  void display_menu_cursor(uint8_t l, uint8_t m);
  void display_menu_config();
  void display_settings(bool USE_CEL, bool SHOW_SPLASH, int SCROLL_SPEED);
  void user_settings(bool AUTO_OFF, double SET_TEMP_BUMP, double SET_TEMP_MAX, uint32_t BUMP_TIME, uint32_t RUN_TIME_MAX);
  void reset_config_settings();

private:
  oLED *oled;

  double MES_TEMP;
  double SET_TEMP;

  double kP;
  double kI;
  double kD;

  void dmpc(uint8_t t);
  void dmpc_b(uint8_t t);
  void dmc(uint8_t t);
  void dmc_b(uint8_t t);
};

extern Display interface;

#endif
