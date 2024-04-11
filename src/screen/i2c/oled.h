#ifndef OLED_H
#define OLED_H

#include <Wire.h>
#include "storage/eepromStor.h"

// Font and Icons imports
#include "gfx/icons.c"
#include "gfx/font6x8.h"
#include "gfx/font8x16.h"

class oLED
{

public:
  void ssd1306_send_byte(uint8_t byte);
  void ssd1306_send_command(uint8_t b);
  void ssd1306_send_command_start(void);
  void ssd1306_setpos(uint8_t x, uint8_t y);
  void ssd1306_char_font6x8(char ch);
  void ssd1306_string_font6x8(char *s);
  void ssd1306_char_f8x16(uint8_t x, uint8_t y, const char ch[]);
  void OLED_draw_bmp(uint8_t *buf, int size_x, int size_y, uint8_t x_l, uint8_t y_l, int s);
  void OLED_reset();
  void OLED_clear(bool rst);
  void OLED_init(EepromStorage *eeprom, bool write);
  void OLED_init(EepromStorage *eeprom, bool write, uint8_t addr);

  void save_splash();
  void load_splash();
  void load_splash01();
  void loading_screen();
  void load_logo_s();

  void save_img0();
  void save_splash_big();
  void save_init_led();
  void save_img1();
  void save_icons();

  void display_power(uint16_t x, uint16_t y);
  void display_power_on(uint16_t x, uint16_t y);
  void display_ad(uint16_t x, uint16_t y);
  void display_lmb(uint16_t x, uint16_t y);
  void display_diva(uint16_t x, uint16_t y);
  void display_divb(uint16_t x, uint16_t y);
  void display_divc(uint16_t x, uint16_t y);
  void display_divd(uint16_t x, uint16_t y);
  void display_okay(uint16_t x, uint16_t y);
  void display_menu(uint16_t x, uint16_t y);
  void display_riarrow(uint16_t x, uint16_t y);
  void display_learrow(uint16_t x, uint16_t y);
  void display_uparrow(uint16_t x, uint16_t y);
  void display_doarrow(uint16_t x, uint16_t y);
  void display_cancel(uint16_t x, uint16_t y);
  void display_bump(uint16_t x, uint16_t y);
  void display_bump_on(uint16_t x, uint16_t y);
  void display_t_c(uint16_t x, uint16_t y);
  void display_t_f(uint16_t x, uint16_t y);
  void display_flame(uint16_t t);

private:
  void OLED_init(bool wr);

  // EEPROM accessor class
  EepromStorage *ee;

  // OLED Screen Address
  uint8_t OLED_I2C_ADDRESS = 0x3C;
  // Control byte
  uint8_t OLED_CONTROL_BYTE_CMD_SINGLE = 0x80;
  uint8_t OLED_CONTROL_BYTE_CMD_STREAM = 0x00;
  uint8_t OLED_CONTROL_BYTE_DATA_STREAM = 0x40;
  // Addressing Command Table (pg.30)
  uint8_t OLED_CMD_SET_MEMORY_ADDR_MODE = 0x20; // follow with 0x00 = HORZ mode = Behave like a KS108 graphic LCD
  uint8_t OLED_CMD_SET_COLUMN_RANGE = 0x21;     // can be used only in HORZ/VERT mode - follow with 0x00 and 0x7F = COL127
  uint8_t OLED_CMD_SET_PAGE_RANGE = 0x22;       // can be used only in HORZ/VERT mode - follow with 0x00 and 0x07 = PAGE7

  void ssd1306_send_data_start(void);
  void ssd1306_send_data_stop(void);
};

extern oLED oled;

#endif
