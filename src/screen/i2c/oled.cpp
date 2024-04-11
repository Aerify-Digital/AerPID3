#include "oled.h"

// ===============================================================
// ===============================================================
// Screen Functions
// =============================================================
void oLED::ssd1306_send_byte(uint8_t byte)
{
  Wire.write(byte);
}

void oLED::ssd1306_send_command(uint8_t b)
{
  oLED::ssd1306_send_command_start();
  oLED::ssd1306_send_byte(b);
  oLED::ssd1306_send_data_stop();
}

void oLED::ssd1306_send_command_start(void)
{
  Wire.beginTransmission(oLED::OLED_I2C_ADDRESS);
  Wire.write(oLED::OLED_CONTROL_BYTE_CMD_STREAM);
}

void oLED::ssd1306_send_data_start(void)
{
  Wire.beginTransmission(oLED::OLED_I2C_ADDRESS);
  Wire.write(oLED::OLED_CONTROL_BYTE_DATA_STREAM);
}

void oLED::ssd1306_send_data_stop(void)
{
  Wire.endTransmission();
}

void oLED::ssd1306_setpos(uint8_t x, uint8_t y)
{
  oLED::ssd1306_send_command_start();
  oLED::ssd1306_send_byte(0xb0 + y);
  oLED::ssd1306_send_byte(((x & 0xf0) >> 4) | 0x10); // | 0x10
  oLED::ssd1306_send_byte((x & 0x0f) | 0x01);        // | 0x01
  oLED::ssd1306_send_data_stop();
}

void oLED::ssd1306_char_font6x8(char ch)
{
  oLED::ssd1306_send_data_start();
  for (int i = 0; i < 6; i++)
  {
    oLED::ssd1306_send_byte(pgm_read_byte(&ssd1306xled_font6x8[(ch - 32) * 6 + i]));
  }
  oLED::ssd1306_send_data_stop();
}

void oLED::ssd1306_string_font6x8(char *s)
{
  while (*s)
  {
    oLED::ssd1306_char_font6x8(*s++);
  }
}

void oLED::ssd1306_char_f8x16(uint8_t x, uint8_t y, const char ch[])
{
  uint8_t c = 0;
  int j = 0;
  while (ch[j] != '\0')
  {
    c = ch[j] - 32;
    if (x > 120)
    {
      x = 0;
      y++;
    }
    oLED::ssd1306_setpos(x, y);
    oLED::ssd1306_send_data_start();
    for (int i = 0; i < 8; i++)
    {
      oLED::ssd1306_send_byte(pgm_read_byte(&ssd1306xled_font8x16[c * 16 + i]));
    }
    ssd1306_send_data_stop();
    ssd1306_setpos(x, y + 1);
    ssd1306_send_data_start();
    for (int i = 0; i < 8; i++)
    {
      oLED::ssd1306_send_byte(pgm_read_byte(&ssd1306xled_font8x16[c * 16 + i + 8]));
    }
    oLED::ssd1306_send_data_stop();
    x += 8;
    j++;
  }
  j = 0;
}

void oLED::OLED_draw_bmp(uint8_t *buf, int size_x, int size_y, uint8_t x_l, uint8_t y_l, int s)
{
  uint8_t y_max = y_l + (size_y / 8) + 1;

  int l = 0;
  int m = 0;
  while (l < s)
  {
    if (m >= y_max)
    {
      m = y_l;
    }
    if (m >= y_l && m <= y_max)
    {
      oLED::ssd1306_setpos(x_l, m);
      oLED::ssd1306_send_data_start();
      for (int x = 0; x < size_x; x++)
      {
        oLED::ssd1306_send_byte(buf[l]);
        l++;
      }
      oLED::ssd1306_send_data_stop();
    }
    m++;
  }
}

void oLED::OLED_reset()
{
  // I2C
  oLED::ssd1306_send_command_start();
  oLED::ssd1306_send_byte(OLED_CMD_SET_COLUMN_RANGE);
  oLED::ssd1306_send_byte(0x00);
  oLED::ssd1306_send_byte(0x7F);
  oLED::ssd1306_send_byte(OLED_CMD_SET_PAGE_RANGE);
  oLED::ssd1306_send_byte(0);
  oLED::ssd1306_send_byte(0x07);
  oLED::ssd1306_send_data_stop();
}

// ======================
// Clear the OLED Display
void oLED::OLED_clear(bool rst)
{
  if (rst)
  {
    oLED::OLED_reset();
  }
  for (int j = 0; j < 1024; j++)
  {
    oLED::ssd1306_send_data_start();
    for (int i = 0; i < 16; i++)
    {
      oLED::ssd1306_send_byte(0x00);
      j++;
    }
    j--;
    oLED::ssd1306_send_data_stop();
  }
}

// ======================================================================
// ======================================================================
const static uint8_t ssd1306_init_sequence[] PROGMEM = {
    // Initialization Sequence
    // const static uint8_t ssd1306_init_sequence[] = {  // Initialization Sequence
    0xAE,       // Display OFF (sleep mode)
    0x20, 0b00, // Set Memory Addressing Mode
                // 00=Horizontal Addressing Mode; 01=Vertical Addressing Mode;
                // 10=Page Addressing Mode (RESET); 11=Invalid
    0xB0,       // Set Page Start Address for Page Addressing Mode, 0-7
    0xC8,       // Set COM Output Scan Direction
    0x00,       // ---set low column address
    0x10,       // ---set high column address
    0x40,       // --set start line address
    0x81, 0x3F, // Set contrast control register
    0xA1,       // Set Segment Re-map. A0=address mapped; A1=address 127 mapped.
    0xA6,       // Set display mode. A6=Normal; A7=Inverse
    0xA8, 0x3F, // Set multiplex ratio(1 to 64)
    0xA4,       // Output RAM to Display
                // 0xA4=Output follows RAM content; 0xA5,Output ignores RAM content
    0xD3, 0x00, // Set display offset. 00 = no offset
    0xD5,       // --set display clock divide ratio/oscillator frequency
    0xF0,       // --set divide ratio
    0xD9, 0x22, // Set pre-charge period
    0xDA, 0x12, // Set com pins hardware configuration
    0xDB,       // --set vcomh
    0x20,       // 0x20,0.77xVcc
    0x8D, 0x14, // Set DC-DC enable
    0xAF        // Display ON in normal mode
};

// ==============================================
// Send Init Command Sequence to the OLED Display
void oLED::OLED_init(EepromStorage *eeprom, bool write)
{
  oLED::ee = eeprom;
  oLED::OLED_init(write);
}

void oLED::OLED_init(EepromStorage *eeprom, bool write, uint8_t addr)
{
  oLED::OLED_I2C_ADDRESS = addr;
  oLED::OLED_init(eeprom, write);
}

void oLED::OLED_init(bool wr)
{
  Serial.print(F("Initializing OLED ... "));

  if (wr)
  {
    oLED::save_splash();
    save_splash_big();
    save_icons();
    save_img0();
    save_img1();

    // save_init_led();
    delay(80);
  }

  uint8_t data[28];

  for (int i = 0; i < 28; i++)
  {
    // data[i] = ee->readByte(800 + i);
    data[i] = pgm_read_byte(&ssd1306_init_sequence[i]);
  }

  for (int i = 0; i < 28; i++)
  {
    oLED::ssd1306_send_command(data[i]);
  }
  Serial.println(F("  Complete!"));
}

// =====================================================================
// =====================================================================

void oLED::save_splash()
{
  int offset = 1000;
  oLED::ee->writeEnable();
  for (int i = 0; i < sizeof(logo000); i++)
  {
    oLED::ee->write(offset + i, (uint8_t)logo000[i]);
  }
  // oLED::ee->write(offset, (uint8_t*) &logo000, sizeof(logo000));
  oLED::ee->writeDisable();
  Serial.print(F("Saved Splash Screen to EEPROM ..."));
  delay(50);
}

void oLED::load_splash()
{
  int offset = 1000;
  oLED::ee->rb(offset, 128);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 32, 48, 2, 128);
}

void oLED::load_splash01()
{
  oLED::OLED_clear(false);

  // Begin Top Header
  // Section A1
  oLED::ee->rb(2000, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 0, 0, 32);
  // Section B1
  oLED::ee->rb(2032, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 32, 0, 32);
  // Section C1
  oLED::ee->rb(2064, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 64, 0, 32);
  // Section D1
  oLED::ee->rb(2096, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 96, 0, 32);
  // End Top Header

  // Section B1a
  oLED::ee->rb(2160, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 32, 1, 32);
  // Section C1a
  oLED::ee->rb(2192, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 64, 1, 32);
  // Section C1b
  oLED::ee->rb(2320, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 64, 2, 32);
  // Section B1b
  oLED::ee->rb(2288, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 32, 2, 32);
  // Section B1c
  oLED::ee->rb(2416, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 32, 3, 32);
  // Section C1c
  oLED::ee->rb(2448, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 64, 3, 32);

  // Section B2a
  oLED::ee->rb(2544, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 32, 4, 32);
  // Section C2a
  oLED::ee->rb(2576, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 64, 4, 32);

  // Section B2b
  oLED::ee->rb(2672, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 32, 5, 32);
  oLED::ee->rb(2800, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 32, 6, 32);
  oLED::ee->rb(2928, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 32, 7, 32);
  // Section C2b
  oLED::ee->rb(2704, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 64, 5, 32);
  oLED::ee->rb(2832, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 64, 6, 32);
  oLED::ee->rb(2960, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 64, 7, 32);
  // Section A1a
  oLED::ee->rb(2128, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 0, 1, 32);
  // Section D1a
  oLED::ee->rb(2224, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 96, 1, 32);
  // Section D1b
  oLED::ee->rb(2352, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 96, 2, 32);
  // Section A1b
  oLED::ee->rb(2256, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 0, 2, 32);
  // Section A1c
  oLED::ee->rb(2384, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 0, 3, 32);
  // Section D1c
  oLED::ee->rb(2480, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 96, 3, 32);
  // Section A2a
  oLED::ee->rb(2512, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 0, 4, 32);
  // Section D2a
  oLED::ee->rb(2608, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 96, 4, 32);

  // Section D2b
  oLED::ee->rb(2736, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 96, 5, 32);
  oLED::ee->rb(2864, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 96, 6, 32);
  oLED::ee->rb(2992, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 96, 7, 32);
  // Section A2b
  oLED::ee->rb(2640, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 0, 5, 32);
  oLED::ee->rb(2768, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 0, 6, 32);
  oLED::ee->rb(2896, 32);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 32, 8, 0, 7, 32);

  delay(70);
}

void oLED::save_img0()
{
  int offset = 1150;
  oLED::ee->writeEnable();
  oLED::ee->write(offset, (uint8_t *)&logo003, sizeof(logo003));
  oLED::ee->writeDisable();
}

void oLED::save_splash_big()
{
  int offset = 2000;
  oLED::ee->writeEnable();
  oLED::ee->write(offset, (uint8_t *)&splash01, sizeof(splash01));
  oLED::ee->writeDisable();
}

void oLED::save_init_led()
{
  uint16_t offset = 800;
  oLED::ee->writeEnable();
  oLED::ee->write(offset, (byte *)&ssd1306_init_sequence, 28);
  oLED::ee->writeDisable();
}

const unsigned char flame00[] = {
    0x78,
    0xFE,
    0xE6,
    0xFB,
    0xF7,
    0x3E,
};

const unsigned char flame01[] = {
    0x10,
    0xF8,
    0xEC,
    0xF7,
    0x7E,
    0x1C,
};

const unsigned char flame02[] = {
    0x18,
    0x64,
    0xC0,
    0xDA,
    0xF6,
    0x0E,
};

const unsigned char flame00_blank[] = {
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
};

void oLED::save_img1()
{
  oLED::ee->writeEnable();
  oLED::ee->write(1310, (uint8_t *)&flame00, sizeof(flame00));             // 6
  oLED::ee->write(1320, (uint8_t *)&flame01, sizeof(flame01));             // 6
  oLED::ee->write(1330, (uint8_t *)&flame02, sizeof(flame02));             // 6
  oLED::ee->write(1300, (uint8_t *)&flame00_blank, sizeof(flame00_blank)); // 6
  oLED::ee->writeDisable();
  delay(20);
}

void oLED::save_icons()
{
  oLED::ee->writeEnable();
  oLED::ee->write(1400, (uint8_t *)&bump, sizeof(bump));             // 12
  oLED::ee->write(1412, (uint8_t *)&cancel, sizeof(cancel));         // 12
  oLED::ee->write(1424, (uint8_t *)&downarrow, sizeof(downarrow));   // 12
  oLED::ee->write(1436, (uint8_t *)&uparrow, sizeof(uparrow));       // 12
  oLED::ee->write(1448, (uint8_t *)&leftarrow, sizeof(leftarrow));   // 12
  oLED::ee->write(1460, (uint8_t *)&rightarrow, sizeof(rightarrow)); // 12
  oLED::ee->write(1472, (uint8_t *)&menu, sizeof(menu));             // 12
  oLED::ee->write(1484, (uint8_t *)&okay, sizeof(okay));             // 12
  oLED::ee->write(1496, (uint8_t *)&power, sizeof(power));           // 12
  oLED::ee->write(1508, (uint8_t *)&pow_on, sizeof(pow_on));         // 12
  oLED::ee->write(1520, (uint8_t *)&lmb0, sizeof(lmb0));             // 12
  oLED::ee->write(1532, (uint8_t *)&div_a, sizeof(div_a));           // 12
  oLED::ee->write(1544, (uint8_t *)&div_b, sizeof(div_b));           // 12
  oLED::ee->write(1556, (uint8_t *)&div_c, sizeof(div_c));           // 12
  oLED::ee->write(1568, (uint8_t *)&div_d, sizeof(div_d));           // 12
  oLED::ee->write(1580, (uint8_t *)&ad, sizeof(ad));                 // 12
  oLED::ee->write(1592, (uint8_t *)&bump_on, sizeof(bump_on));       // 12
  oLED::ee->write(1604, (uint8_t *)&t_c, sizeof(t_c));               // 8
  oLED::ee->write(1612, (uint8_t *)&t_f, sizeof(t_f));               // 8
  oLED::ee->writeDisable();
  delay(20);
}

void oLED::load_logo_s()
{
  oLED::ee->rb(1150, 72);
  // OLED_draw_bmp(data , 24, 24, 52, 3, 72);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 24, 24, 4, 5, 72);
}

// =====================================================================
// Display Icons

void oLED::display_power(uint16_t x, uint16_t y)
{
  oLED::ee->rb(1496, 12);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 12, 8, x, y, 12);
}

void oLED::display_power_on(uint16_t x, uint16_t y)
{
  oLED::ee->rb(1508, 12);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 12, 8, x, y, 12);
}

void oLED::display_ad(uint16_t x, uint16_t y)
{
  oLED::ee->rb(1580, 12);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 12, 8, x, y, 12);
}

void oLED::display_lmb(uint16_t x, uint16_t y)
{
  oLED::ee->rb(1520, 12);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 12, 8, x, y, 12);
}

void oLED::display_diva(uint16_t x, uint16_t y)
{
  oLED::ee->rb(1532, 12);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 12, 8, x, y, 12);
}

void oLED::display_divb(uint16_t x, uint16_t y)
{
  oLED::ee->rb(1544, 12);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 12, 8, x, y, 12);
}

void oLED::display_divc(uint16_t x, uint16_t y)
{
  oLED::ee->rb(1556, 12);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 12, 8, x, y, 12);
}

void oLED::display_divd(uint16_t x, uint16_t y)
{
  oLED::ee->rb(1568, 12);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 12, 8, x, y, 12);
}

void oLED::display_okay(uint16_t x, uint16_t y)
{
  oLED::ee->rb(1484, 12);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 12, 8, x, y, 12);
}

void oLED::display_menu(uint16_t x, uint16_t y)
{
  oLED::ee->rb(1472, 12);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 12, 8, x, y, 12);
}

void oLED::display_riarrow(uint16_t x, uint16_t y)
{
  oLED::ee->rb(1460, 12);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 12, 8, x, y, 12);
}

void oLED::display_learrow(uint16_t x, uint16_t y)
{
  oLED::ee->rb(1448, 12);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 12, 8, x, y, 12);
}

void oLED::display_uparrow(uint16_t x, uint16_t y)
{
  oLED::ee->rb(1436, 12);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 12, 8, x, y, 12);
}

void oLED::display_doarrow(uint16_t x, uint16_t y)
{
  oLED::ee->rb(1424, 12);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 12, 8, x, y, 12);
}

void oLED::display_cancel(uint16_t x, uint16_t y)
{
  oLED::ee->rb(1412, 12);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 12, 8, x, y, 12);
}

void oLED::display_bump(uint16_t x, uint16_t y)
{
  oLED::ee->rb(1400, 12);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 12, 8, x, y, 12);
}

void oLED::display_bump_on(uint16_t x, uint16_t y)
{
  oLED::ee->rb(1592, 12);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 12, 8, x, y, 12);
}

void oLED::display_t_c(uint16_t x, uint16_t y)
{
  oLED::ee->rb(1604, 8);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 8, 8, x, y, 8);
}

void oLED::display_t_f(uint16_t x, uint16_t y)
{
  oLED::ee->rb(1612, 8);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 8, 8, x, y, 8);
}

void oLED::display_flame(uint16_t t)
{
  oLED::ee->rb(t, 6);
  oLED::OLED_draw_bmp((uint8_t *)oLED::ee->getData(), 6, 8, 3, 1, 6);
}

// =====================================================
void oLED::loading_screen()
{
  // Clear..
  oLED::OLED_clear(false);
  // Loading...
  oLED::ssd1306_setpos(35, 0);
  oLED::ssd1306_string_font6x8("Loading...");
  // Load the loading splash image
  oLED::load_splash();
  delay(60);
  // Vendor...
  oLED::ssd1306_setpos(41, 6);
  oLED::ssd1306_string_font6x8("AerLite");
  oLED::ssd1306_setpos(25, 7);
  oLED::ssd1306_string_font6x8("AerifyDigital");
  delay(420);
}

// Setup the OLED
oLED oled;
