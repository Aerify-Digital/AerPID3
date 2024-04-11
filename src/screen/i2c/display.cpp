#include "display.h"

void Display::init(oLED *led)
{
  Display::oled = led;
  Serial.println("Display Initialized!");
}

// ===============================================================
// ===============================================================
// ===============================================================
// Display Mains

void Display::display_blank(int i, int j)
{
  Display::oled->ssd1306_setpos(i, j);
  Display::oled->ssd1306_string_font6x8("     ");
}

void Display::display_blank(int i, int j, int a)
{
  Display::oled->ssd1306_setpos(i, j);
  if (a == 5)
    Display::oled->ssd1306_string_font6x8("     ");
  if (a == 6)
    Display::oled->ssd1306_string_font6x8("      ");
}

void Display::setPID(double p, double i, double d)
{
  Display::kP = p;
  Display::kI = i;
  Display::kD = d;
}

void Display::display_main(bool USE_CEL, bool PID_ON, bool EN_BUMP, double TEMP_M, double TEMP_S)
{
  Display::oled->OLED_clear(true);

  Display::oled->ssd1306_char_f8x16(14, 0, "TEMP:");
  Display::display_set_temp(TEMP_S);

  if (USE_CEL)
  {
    Display::oled->display_t_c(108, 0);
  }
  else
  {
    Display::oled->display_t_f(108, 0);
  }

  Display::oled->ssd1306_char_f8x16(14, 3, "SET:");
  Display::display_mes_temp(TEMP_M);

  if (USE_CEL)
  {
    Display::oled->display_t_c(108, 3);
  }
  else
  {
    Display::oled->display_t_f(108, 3);
  }

  if (PID_ON)
  {
    Display::oled->display_power_on(5, 7);
  }
  else
  {
    Display::oled->display_power(5, 7);
  }
  if (EN_BUMP)
  {
    Display::oled->display_bump_on(29, 7);
  }
  else
  {
    Display::oled->display_bump(29, 7);
  }
  Display::oled->display_uparrow(53, 7);
  Display::oled->display_doarrow(77, 7);
  Display::oled->display_menu(101, 7);
}

// =================================================================
// PID MENU
// =================================================================
void Display::display_menu_pid(uint8_t i)
{
  Display::oled->OLED_clear(false);

  Display::oled->ssd1306_setpos(2, 0);
  Display::oled->ssd1306_string_font6x8("=== PID SETTINGS ===");

  Display::oled->ssd1306_setpos(20, 2);
  Display::oled->ssd1306_string_font6x8("P =");
  Display::oled->ssd1306_setpos(20, 3);
  Display::oled->ssd1306_string_font6x8("I =");
  Display::oled->ssd1306_setpos(20, 4);
  Display::oled->ssd1306_string_font6x8("D =");

  Display::oled->display_learrow(5, 7); // --/++/DOWN/UP/EXIT
  Display::oled->display_riarrow(29, 7);
  Display::oled->display_uparrow(77, 7);
  Display::oled->display_doarrow(53, 7);
  Display::oled->display_okay(101, 7);

  Display::display_menu_pid_cursor(i);

  Display::display_menu_pid_kP();
  Display::display_menu_pid_kI();
  Display::display_menu_pid_kD();
}

void Display::display_menu_pid_kP()
{
  char b[8];
  Display::display_blank(50, 2, 6);
  Display::oled->ssd1306_setpos(50, 2);
  dtostrf(Display::kP, 6, 3, b);
  Display::oled->ssd1306_string_font6x8(b);
}

void Display::display_menu_pid_kI()
{
  char b[8];
  Display::display_blank(50, 3, 6);
  Display::oled->ssd1306_setpos(50, 3);
  dtostrf(Display::kI, 6, 4, b);
  Display::oled->ssd1306_string_font6x8(b);
}

void Display::display_menu_pid_kD()
{
  char b[8];
  Display::display_blank(50, 4, 6);
  Display::oled->ssd1306_setpos(50, 4);
  dtostrf(Display::kD, 6, 3, b);
  Display::oled->ssd1306_string_font6x8(b);
}

void Display::display_menu_pid_cursor(uint16_t s)
{
  if (s == 1)
  {
    Display::dmpc(2);
    Display::dmpc_b(3);
    Display::dmpc_b(4);
  }
  else if (s == 2)
  {
    Display::dmpc_b(2);
    Display::dmpc(3);
    Display::dmpc_b(4);
  }
  else if (s == 3)
  {
    Display::dmpc_b(2);
    Display::dmpc_b(3);
    Display::dmpc(4);
  }
}

void Display::dmpc(uint8_t t)
{
  Display::oled->ssd1306_setpos(10, t);
  Display::oled->ssd1306_string_font6x8(">");
}

void Display::dmpc_b(uint8_t t)
{
  Display::oled->ssd1306_setpos(10, t);
  Display::oled->ssd1306_string_font6x8(" ");
}

// =================================================================
// Connections MENU
// =================================================================
void Display::display_menu_con()
{
  Display::oled->OLED_clear(false);
  Display::display_con_h();

  Display::oled->ssd1306_setpos(2, 1);
  Display::oled->ssd1306_string_font6x8("> Connect");

  Display::oled->display_okay(77, 7);
  Display::oled->display_cancel(101, 7);
}

void Display::display_menu_con_go()
{
  Display::oled->OLED_clear(false);
  Display::display_con();
  Display::oled->display_cancel(101, 7);
}

void Display::display_con_h()
{
  Display::oled->ssd1306_setpos(2, 0);
  Display::oled->ssd1306_string_font6x8("=== CONNECTIVITY ===");
}

void Display::display_con()
{
  Display::display_con_h();
  Display::oled->ssd1306_setpos(2, 1);
  Display::oled->ssd1306_string_font6x8("> Connecting...");
}

void Display::display_blu_connect_okay()
{
  // OLED_clear(true);
  // display_con();

  Display::oled->ssd1306_setpos(2, 3);
  Display::oled->ssd1306_string_font6x8(">> Success!");

  Display::oled->display_okay(101, 7);
}

void Display::display_blu_connect_fail()
{
  // OLED_clear(true);
  // display_con();

  Display::oled->ssd1306_setpos(2, 3);
  Display::oled->ssd1306_string_font6x8(">> Failed!");

  Display::oled->display_okay(101, 7);
}

// =================================================================
// Display Temperature
// =================================================================
void Display::display_temp(int i)
{
  char b[8];
  if (i == 0)
  {
    dtostrf(Display::MES_TEMP, 6, 1, b);
  }
  else if (i == 3)
  {
    dtostrf(Display::SET_TEMP, 6, 1, b);
  }
  // ssd1306_string_font6x8(b);
  Display::oled->ssd1306_char_f8x16(52, i, b);
}

void Display::display_set_temp(double temp)
{
  Display::SET_TEMP = temp;
  Display::display_temp(3);
}

void Display::display_mes_temp(double temp)
{
  Display::MES_TEMP = temp;
  Display::display_temp(0);
}

// =================================================================
// Main Menu
// =================================================================
void Display::display_menu_home(uint8_t l)
{
  Display::oled->OLED_clear(false);

  Display::oled->display_diva(0, 0);
  Display::oled->ssd1306_setpos(34, 0);
  Display::oled->ssd1306_string_font6x8("MENU MAIN");
  Display::oled->display_diva(108, 0);
  Display::oled->ssd1306_setpos(10, 1);
  Display::oled->ssd1306_string_font6x8("PID Settings");
  Display::oled->ssd1306_setpos(10, 2);
  Display::oled->ssd1306_string_font6x8("Connectivity");
  Display::oled->ssd1306_setpos(10, 3);
  Display::oled->ssd1306_string_font6x8("Configuration");
  Display::oled->ssd1306_setpos(10, 4);
  Display::oled->ssd1306_string_font6x8("Exit/Cancel");

  Display::display_menu_cursor(l, 4);

  Display::oled->display_uparrow(77, 7);
  Display::oled->display_doarrow(53, 7);
  Display::oled->display_okay(101, 7);

  Display::oled->load_logo_s();
}

void Display::dmc(uint8_t t)
{
  Display::oled->ssd1306_setpos(0, t);
  Display::oled->ssd1306_string_font6x8(">");
}

void Display::dmc_b(uint8_t t)
{
  Display::oled->ssd1306_setpos(0, t);
  Display::oled->ssd1306_string_font6x8(" ");
}

void Display::display_menu_cursor(uint8_t l, uint8_t m)
{
  for (int i = 1; i < m + 1; i++)
  {
    if (i == l)
    {
      Display::dmc(i);
    }
    else
    {
      Display::dmc_b(i);
    }
  }
}

// =================================================================
// Configuration MENU
// =================================================================
void Display::display_menu_config()
{
  Display::oled->OLED_clear(false);

  Display::oled->display_uparrow(77, 7);
  Display::oled->display_doarrow(53, 7);
  Display::oled->display_cancel(101, 7);
  Display::oled->display_okay(5, 7);

  Display::oled->ssd1306_setpos(0, 0);
  Display::oled->ssd1306_string_font6x8("=== CONFIGURATION ===");
  Display::oled->ssd1306_setpos(2, 1);
  Display::oled->ssd1306_string_font6x8("> User Settings");
  Display::oled->ssd1306_setpos(2, 2);
  Display::oled->ssd1306_string_font6x8("> Display Settings");
  Display::oled->ssd1306_setpos(2, 3);
  Display::oled->ssd1306_string_font6x8("> Reset To Defaults");
  Display::oled->ssd1306_setpos(2, 4);
  Display::oled->ssd1306_string_font6x8("> HARDWARE INFO");

  Display::display_menu_cursor(1, 4);
}

// =================================================================
// Display Options
// =================================================================
void Display::display_settings(bool USE_CEL, bool SHOW_SPLASH, int SCROLL_SPEED)
{
  Display::oled->OLED_clear(false);

  Display::oled->display_diva(0, 0);
  Display::oled->ssd1306_setpos(14, 0);
  Display::oled->ssd1306_string_font6x8("DISPLAY SETTING");
  Display::oled->display_diva(108, 0);
  Display::oled->ssd1306_setpos(10, 1);
  Display::oled->ssd1306_string_font6x8("Temperature");
  Display::oled->ssd1306_setpos(10, 2);
  Display::oled->ssd1306_string_font6x8("Scroll Speed");
  Display::oled->ssd1306_setpos(10, 3);
  Display::oled->ssd1306_string_font6x8("Show Splash");
  // ssd1306_setpos(10, 4);
  // ssd1306_string_font6x8("DERP");

  if (USE_CEL)
  {
    Display::oled->display_t_c(94, 1);
  }
  else
  {
    Display::oled->display_t_f(94, 1);
  }

  char b[8];
  Display::oled->ssd1306_setpos(94, 2);
  dtostrf(SCROLL_SPEED, 1, 0, b);
  Display::oled->ssd1306_string_font6x8(b);

  if (SHOW_SPLASH)
  {
    Display::oled->ssd1306_setpos(94, 3);
    Display::oled->ssd1306_string_font6x8("YES");
  }
  else
  {
    Display::oled->ssd1306_setpos(94, 3);
    Display::oled->ssd1306_string_font6x8("NO ");
  }

  Display::display_menu_cursor(1, 3);

  Display::oled->display_learrow(5, 7);
  Display::oled->display_riarrow(29, 7);
  Display::oled->display_uparrow(77, 7);
  Display::oled->display_doarrow(53, 7);
  Display::oled->display_okay(101, 7);
}

// =================================================================
// User Options
// =================================================================
void Display::user_settings(bool AUTO_OFF, double SET_TEMP_BUMP, double SET_TEMP_MAX, uint32_t BUMP_TIME, uint32_t RUN_TIME_MAX)
{
  Display::oled->OLED_clear(false);

  Display::oled->display_diva(0, 0);
  Display::oled->ssd1306_setpos(18, 0);
  Display::oled->ssd1306_string_font6x8("USER SETTINGS");
  Display::oled->display_diva(108, 0);

  Display::oled->ssd1306_setpos(10, 1);
  Display::oled->ssd1306_string_font6x8("Auto-Shutoff");
  Display::oled->ssd1306_setpos(10, 2);
  Display::oled->ssd1306_string_font6x8("Bump Amount");
  Display::oled->ssd1306_setpos(10, 3);
  Display::oled->ssd1306_string_font6x8("Max Temp");
  Display::oled->ssd1306_setpos(10, 4);
  Display::oled->ssd1306_string_font6x8("Bump Time");
  Display::oled->ssd1306_setpos(10, 5);
  Display::oled->ssd1306_string_font6x8("Shutoff Time");

  Display::oled->ssd1306_setpos(94, 1);
  if (AUTO_OFF)
  {
    Display::oled->ssd1306_string_font6x8("ENB");
  }
  else
  {
    Display::oled->ssd1306_string_font6x8("DIS");
  }
  Display::oled->ssd1306_setpos(94, 2);
  char b[8];
  dtostrf(SET_TEMP_BUMP, 2, 0, b);
  Display::oled->ssd1306_string_font6x8(b);
  Display::oled->ssd1306_setpos(94, 3);
  dtostrf(SET_TEMP_MAX, 2, 0, b);
  Display::oled->ssd1306_string_font6x8(b);
  Display::oled->ssd1306_setpos(94, 4);
  dtostrf(BUMP_TIME, 1, 0, b);
  Display::oled->ssd1306_string_font6x8(b);
  Display::oled->ssd1306_setpos(94, 5);
  dtostrf(RUN_TIME_MAX, 1, 0, b);
  Display::oled->ssd1306_string_font6x8(b);

  Display::display_menu_cursor(1, 5);

  Display::oled->display_learrow(5, 7);
  Display::oled->display_riarrow(29, 7);
  Display::oled->display_uparrow(77, 7);
  Display::oled->display_doarrow(53, 7);
  Display::oled->display_okay(101, 7);
}

// =================================================================
// RESET
// =================================================================
void Display::reset_config_settings()
{
  Display::oled->OLED_clear(false);

  Display::oled->display_diva(0, 0);
  Display::oled->display_divd(12, 0);
  Display::oled->ssd1306_setpos(44, 0);
  Display::oled->ssd1306_string_font6x8("RESET");
  Display::oled->display_divd(96, 0);
  Display::oled->display_diva(108, 0);

  Display::oled->ssd1306_setpos(0, 2);
  Display::oled->ssd1306_string_font6x8("This will reset your");
  Display::oled->ssd1306_setpos(0, 3);
  Display::oled->ssd1306_string_font6x8("Device to Defaults..");
  Display::oled->ssd1306_setpos(66, 5);
  Display::oled->ssd1306_string_font6x8("Continue?");

  Display::oled->display_okay(77, 7);
  Display::oled->display_cancel(101, 7);
}

// Setup the Interface (Display)
Display interface;
