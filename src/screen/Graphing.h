
#ifndef _AER_GRAPHING_H
#define _AER_GRAPHING_H

#include "stdio.h"
#include <TFT_eSPI.h>

#include "gui/gui.h"
#include "util/color565.hpp"

#define LTBLUE 0xB6DF
#define LTTEAL 0xBF5F
#define LTGREEN 0xBFF7
#define LTCYAN 0xC7FF
#define LTRED 0xFD34
#define LTMAGENTA 0xFD5F
#define LTYELLOW 0xFFF8
#define LTORANGE 0xFE73
#define LTPINK 0xFDDF
#define LTPURPLE 0xCCFF
#define LTGREY 0xE71C

#define BLUE 0x001F
#define TEAL 0x0438
#define GREEN 0x07E0
#define CYAN 0x07FF
#define RED 0xF800
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define ORANGE 0xFC00
#define PINK 0xF81F
#define PURPLE 0x8010
#define GREY 0xC618
#define WHITE 0xFFFF
#define BLACK 0x0000

#define DKBLUE 0x000D
#define DKTEAL 0x020C
#define DKGREEN 0x03E0
#define DKCYAN 0x03EF
#define DKRED 0x6000
#define DKMAGENTA 0x8008
#define DKYELLOW 0x8400
#define DKORANGE 0x8200
#define DKPINK 0x9009
#define DKPURPLE 0x4010
#define DKGREY 0x4A49

class AerChart
{
public:
    void Graph(AerGUI *gui, double x, double y, byte dp,
               double gx, double gy, double w, double h,
               double xlo, double xhi, double xinc,
               double ylo, double yhi, double yinc,
               const char *title, const char *xlabel, const char *ylabel,
               bool &redraw, unsigned int color, unsigned int bcolor);
    void Trace(TFT_eSprite *spr, double x, double y1, double y2, byte dp,
               double gx, double gy,
               double w, double h,
               double xlo, double xhi, double xinc,
               double ylo, double yhi, double yinc,
               const char *title, const char *xlabel, const char *ylabel,
               bool &update1, unsigned int color, unsigned int bcolor);
    void Trace(TFT_eSprite *spr, double x,
               double y1, double y2, double y3, double y4,
               byte dp,
               double gx, double gy,
               double w, double h,
               double xlo, double xhi, double xinc,
               double ylo, double yhi, double yinc,
               const char *title, const char *xlabel, const char *ylabel,
               bool &update1, unsigned int bcolor);

private:
    double ox = -999;
    double oy1 = -999; // Force them to be off screen
    double oy2 = -999; // Force them to be off screen
    double oy3 = -999; // Force them to be off screen
    double oy4 = -999; // Force them to be off screen

    bool display1 = true;
    bool update1 = true;
};

#endif