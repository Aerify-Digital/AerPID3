#ifndef tftLCD_SPI_H_
#define tftLCD_SPI_H_

// #include <FreeRTOS.h>
// #include <task.h>

#include <Adafruit_GFX.h>

#include "common/datatypes/mutex.h"
#include "util/color565.hpp"

#include "screen/st7789.h"
#include "screen/gui/gui.h"
#include "core/controllers/AerManager.h"

// Controls Flags..
#define SHOW_SCREEN_FPS 0     // Prints the current FPS to debug. 1 to enable.
#define STARFIELD_STREAMS 1   // Show streaming star effect in starfield. 1 to enable.
#define STARFIELD_STEAM_GAP 2 // Gap between pixels to create streaming stars, or speed

// TFT size
static uint16_t width = TFT_WIDTH;   // 240 pixels
static uint16_t height = TFT_HEIGHT; // 320 pixels

// Starfield center position.  0.5 = middle
float posX = 0.50;
float posY = 0.50;

int xWidthMax = (double)width * posX;
int xWidthMin = (double)width - xWidthMax;
int xHeightMax = (double)height * posY;
int xHeightMin = (double)height - xHeightMax;

// #define NSTARS 384
//  uint8_t sx[NSTARS] = {};
//  uint8_t sy[NSTARS] = {};
//  uint8_t sz[NSTARS] = {};

uint8_t *sx = (uint8_t *)malloc(sizeof(uint8_t) * 0);
uint8_t *sy = (uint8_t *)malloc(sizeof(uint8_t) * 0);
uint8_t *sz = (uint8_t *)malloc(sizeof(uint8_t) * 0);

uint8_t za, zb, zc, zx;

// ==========================================================
// Function declares

void tft_demo(void *pvParameters);
static void setupStarfield(void *pvParameters);
void loopStarfield(TFT_eSPI *tftLCD, uint8_t *sx, uint8_t *sy, uint8_t *sz, uint16_t nStars);

void dispose_starfieldData()
{
  free(sx);
  free(sy);
  free(sz);
  sx = (uint8_t *)malloc(sizeof(uint8_t) * 0);
  sy = (uint8_t *)malloc(sizeof(uint8_t) * 0);
  sz = (uint8_t *)malloc(sizeof(uint8_t) * 0);
}

// ==========================================================
// ==========================================================
// Functions
// ==========================================================

// Fast 0-255 random number generator from http://eternityforest.com/Projects/rng.php:
uint8_t __attribute__((always_inline)) rng()
{
  zx++;
  za = (za ^ zc ^ zx);
  zb = (zb + za);
  zc = ((zc + (zb >> 1)) ^ za);
  return zc;
}

void tft_demo(void *pvParameters)
{
  AerManager *_am = (AerManager *)pvParameters;
  AerGUI *_aerGUI = _am->getGUI();
  TFT_eSPI *tft = _aerGUI->getTFT();
  if (xSemaphoreTake(spi1_mutex, 500) == pdTRUE)
  {
    tft->fillScreen(TFT_BLACK);
    xSemaphoreGive(spi1_mutex);
  }
  // printf("Starting TFT Starfield Task...");
  setupStarfield(pvParameters);
}

void fill_test(TFT_eSPI tft, TFT_eSprite spr)
{
  int WIDTH = 240;
  int HEIGHT = 320;

  spr.createSprite(40, 40);

  // Fill the whole sprite with black (Sprite is in memory so not visible yet)
  spr.fillSprite(TFT_BLUE);

  // Number of pixels to draw
  uint16_t n = 100;

  // Draw 100 random colour pixels at random positions in sprite
  while (n--)
  {
    uint16_t colour = random(0x10000); // Returns colour 0 - 0xFFFF
    int16_t x = random(WIDTH);         // Random x coordinate
    int16_t y = random(HEIGHT);        // Random y coordinate
    spr.drawPixel(x, y, colour);       // Draw pixel in sprite
  }

  // Draw some lines
  spr.drawLine(1, 0, WIDTH, HEIGHT - 1, TFT_GREEN);
  spr.drawLine(0, 0, WIDTH, HEIGHT, TFT_GREEN);
  spr.drawLine(0, 1, WIDTH - 1, HEIGHT, TFT_GREEN);
  spr.drawLine(0, HEIGHT - 1, WIDTH - 1, 0, TFT_RED);
  spr.drawLine(0, HEIGHT, WIDTH, 0, TFT_RED);
  spr.drawLine(1, HEIGHT, WIDTH, 1, TFT_RED);

  // Draw some text with Middle Centre datum
  spr.setTextDatum(MC_DATUM);
  spr.drawString("Sprite", WIDTH / 2, HEIGHT / 2, 4);

  // Now push the sprite to the TFT at position 0,0 on screen
  // spr.pushSprite(-40, -40);
  spr.pushSprite(40, 40);
  spr.pushSprite(tft.width() / 2 - WIDTH / 2, tft.height() / 2 - HEIGHT / 2);
  spr.pushSprite(tft.width() - WIDTH + 40, tft.height() - HEIGHT + 40);
}

void drawLogoSprite(TFT_eSprite spr1, TFT_eSprite spr2)
{
  int x1 = 128;
  int y1 = 64;
  int x2 = 115;
  int y2 = 128;

  spr1.createSprite(64, 64);
  spr1.pushImage(0, 0, 64, 64, image_data_aeri_logo);
  spr1.pushSprite(x1, y1, 0x0000);

  spr2.createSprite(96, 32);
  spr2.setTextColor(spr2.color565(192, 210, 200));
  spr2.setTextSize(1);
  spr2.drawString("AerTiny", 0, 0, 4);
  spr2.pushSprite(x2, y2);

  // img.createSprite(128, 32);
  // img.setTextColor(TFT_BLUE, TFT_BLACK);
  // img.drawString("Hello World!", 0, 0, 4);
  // img.pushSprite(64, 64);
}

// draw a string to the screen as a sprite
void drawTextStringSprite(TFT_eSprite spr, String text, int x, int y, int l, int h, int size, uint16_t color)
{
  spr.createSprite(l, h);
  spr.setTextColor(color, TFT_BLACK);
  spr.setTextSize(size);
  spr.drawString(text, 0, 0, 4);
  spr.pushSprite(x, y);
}

// Starfield task
void setupStarfield(void *pvParameters)
{
  AerManager *_am = (AerManager *)pvParameters; // task parameters
  AerGUI *_aerGUI = _am->getGUI();
  TFT_eSPI *tftLCD = _aerGUI->getTFT();

  za = random(256);
  zb = random(256);
  zc = random(256);
  zx = random(256);

  uint16_t _n_stars = _am->getSettings()->getScrnSaverStarCount();
  uint16_t n_stars = std::min(1024, std::max((int)_n_stars, 16));

  sx = (uint8_t *)malloc(sizeof(uint8_t) * n_stars);
  sy = (uint8_t *)malloc(sizeof(uint8_t) * n_stars);
  sz = (uint8_t *)malloc(sizeof(uint8_t) * n_stars);

  while (1)
  {
    if (_am->isScreenSaverActive())
    {
      if (xSemaphoreTake(sys1_mutex, 10) == pdTRUE)
      {
        if (xSemaphoreTake(spi1_mutex, 5) == pdTRUE)
        {
          loopStarfield(tftLCD, sx, sy, sz, n_stars);

          // drawLogoSprite(*_aerGUI->getSpriteBuffer(0), *_aerGUI->getSpriteBuffer(1));

          xSemaphoreGive(spi1_mutex);
        }
        xSemaphoreGive(sys1_mutex);
      }
      vTaskDelay(25 / portTICK_PERIOD_MS);
    }
    else
    {
      vTaskDelay(1050);
    }
  }
  Serial.println("Stopping tft Demo Task...");
  sx[n_stars] = {};
  sy[n_stars] = {};
  sz[n_stars] = {};
  vTaskDelete(NULL);
}

// ==========================================================

void rgbWheel(byte WheelPos, uint8_t *r, uint8_t *g, uint8_t *b)
{
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85)
  {
    *r = 255 - WheelPos * 3;
    *g = 0;
    *b = WheelPos * 3;
    return;
  }
  if (WheelPos < 170)
  {
    WheelPos -= 85;
    *r = 0;
    *g = WheelPos * 3;
    *b = 255 - WheelPos * 3;
    return;
  }
  WheelPos -= 170;
  *r = WheelPos * 3;
  *g = 255 - WheelPos * 3;
  *b = 0;
}

// ==========================================================

// Starfield work loop
void loopStarfield(TFT_eSPI *tftLCD, uint8_t *sx, uint8_t *sy, uint8_t *sz, uint16_t nStars)
{
#if SHOW_SCREEN_FPS == 1
  unsigned long t0 = micros();
#endif

  uint8_t spawnDepthVariation = 255;
  uint8_t gap = STARFIELD_STEAM_GAP;

  for (int i = 0; i < nStars; ++i)
  {
    if (sz[i] <= 1)
    {
      sx[i] = xHeightMax - xWidthMax + rng();
      sy[i] = rng();
      sz[i] = spawnDepthVariation--;
    }
    else
    {
      int old_screen_x = ((int)sx[i] - xHeightMin) * 256 / sz[i] + xHeightMax;
      int old_screen_y = ((int)sy[i] - xWidthMin) * 256 / sz[i] + xWidthMax;

      // This is a faster pixel drawing function for occassions where many single pixels must be drawn
      tftLCD->drawPixel(old_screen_x, old_screen_y, TFT_BLACK);

      sz[i] -= gap;
      if (sz[i] > 1)
      {
        int screen_x = ((int)sx[i] - xHeightMin) * 256 / sz[i] + xHeightMax;
        int screen_y = ((int)sy[i] - xWidthMin) * 256 / sz[i] + xWidthMax;

        if (screen_x >= 0 && screen_y >= 0 && screen_x < height && screen_y < width)
        {
          uint8_t r, g, b;
          // r = g = b = 255 - sz[i];
          rgbWheel((sz[i]) * 512L / 240, &r, &g, &b);
          r = max(0, min(r - sz[i], 255));
          g = max(0, min(g - sz[i], 255));
          b = max(0, min(b - sz[i], 255));

          tftLCD->drawPixel(screen_x, screen_y, tftLCD->color565(b, g, r));
        }
        else
        {
          sz[i] = 0; // Out of screen, die.
        }
      }

#if STARFIELD_STREAMS == 1
      uint8_t _sz = sz[i] - gap;
      if (_sz > 1 && _sz > 28)
      {
        int screen_x = ((int)sx[i] - xHeightMin) * 256 / _sz + xHeightMax;
        int screen_y = ((int)sy[i] - xWidthMin) * 256 / _sz + xWidthMax;

        if (screen_x >= 0 && screen_y >= 0 && screen_x < height && screen_y < width)
        {
          uint8_t r, g, b;
          // r = g = b = 255 - _sz;
          rgbWheel((_sz) * 512L / 240, &r, &g, &b);
          // g = b = 255 - _sz;
          r = max(0, min((r * 1) - (_sz), 255));
          g = max(0, min((g * 1) - (_sz), 255));
          b = max(0, min((b * 1) - (_sz), 255));

          tftLCD->drawPixel(screen_x, screen_y, tftLCD->color565(b, g, r));
        }
      }

      _sz = sz[i] - gap * 1;
      if (_sz > 1 && _sz > 64)
      {
        int screen_x = ((int)sx[i] - xHeightMin) * 256 / _sz + xHeightMax;
        int screen_y = ((int)sy[i] - xWidthMin) * 256 / _sz + xWidthMax;

        if (screen_x >= 0 && screen_y >= 0 && screen_x < height && screen_y < width)
        {
          uint8_t r, g, b;
          r = g = b = max(0, 255 - _sz);
          tftLCD->drawPixel(screen_x, screen_y, tftLCD->color565(b, g, r));
        }
      }
#endif
    }
  }

#if SHOW_SCREEN_FPS == 1
  unsigned long t1 = micros();
  // Calcualte frames per second
  // double fps = 1.0 / ((t1 - t0) / 1000000.0);
  // printf("%f FPS\n", 1.0 / ((t1 - t0) / 1000000.0));
  Message msg;
  snprintf(msg.body, 128, "%f FPS\n", 1.0 / ((t1 - t0) / 1000000.0));
  xQueueSend(usbQueue, (void *)&msg, 10);
#endif
}

#endif