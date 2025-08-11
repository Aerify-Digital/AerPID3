#include "Graphing.h"

/*

  function to draw a cartesian coordinate system and plot whatever data you want
  just pass x and y and the graph will be drawn

  huge arguement list
  &d name of your display object
  x = x data point
  y = y datapont
  gx = x graph location (lower left)
  gy = y graph location (lower left)
  w = width of graph
  h = height of graph
  xlo = lower bound of x axis
  xhi = upper bound of x asis
  xinc = division of x axis (distance not count)
  ylo = lower bound of y axis
  yhi = upper bound of y asis
  yinc = division of y axis (distance not count)
  title = title of graph
  xlabel = x asis label
  ylabel = y asis label
  &redraw = flag to redraw graph on first call only
  color = plotted trace colour
*/

void AerChart::Graph(AerGUI *gui, double x, double y1, double y2, byte dp,
                     double gx, double gy, double w, double h,
                     double xlo, double xhi, double xinc,
                     double ylo, double yhi, double yinc,
                     const char *title, const char *xlabel, const char *ylabel,
                     bool &redraw, unsigned int color1, unsigned int color2, unsigned int bcolor)
{
    TFT_eSPI *tft = gui->getTFT();
    TFT_eSprite *spr = gui->getSpriteBuffer(0);

    // double ydiv, xdiv;
    double i;
    double temp;
    // int rot, newrot;

    // gcolor = graph grid colors
    // acolor = axes line colors
    // pcolor = color of your plotted data
    // tcolor = text color
    // bcolor = background color
    unsigned int gcolor = DKBLUE;
    unsigned int acolor = RED;
    unsigned int pcolor1 = color1;
    unsigned int pcolor2 = color2;
    unsigned int tcolor = WHITE;
    // unsigned int bcolor = BLACK;

    if (redraw == true)
    {

        redraw = false;
        // initialize old x and old y in order to draw the first point of the graph
        // but save the transformed value
        // note my transform funcition is the same as the map function, except the map uses long and we need doubles
        // ox = (x - xlo) * ( w) / (xhi - xlo) + gx;
        // oy1 = (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;

        spr->createSprite(36, h + 10);
        spr->fillRect(0, 0, 36, h + 10, bcolor);

        spr->setTextSize(1);
        spr->setTextDatum(MR_DATUM);

        // draw y scale
        for (i = ylo; i <= yhi; i += yinc)
        {
            // compute the transform
            temp = (i - ylo) * (gy - h - gy) / (yhi - ylo) + gy;

            if (i == 0)
            {
                tft->drawLine(gx, temp, gx + w, temp, acolor);
                tft->setTextColor(acolor, bcolor);
                tft->drawString(xlabel, (int)(gx + w), (int)temp, 2);
            }
            /*else
            {
                tft->drawLine(gx, temp, gx + w, temp, gcolor);
            }*/
            // draw the axis labels
            spr->setTextColor(tcolor, bcolor);

            temp = (i - ylo) * (10 - h - 10) / (yhi - ylo) + h;
            // precision is default Arduino--this could really use some format control
            spr->drawFloat(i, dp, 36, temp, 1);
        }

        /*if (yhi > 0) 
        {
            spr->setTextColor(tcolor, bcolor);
            double yp1 =  (yhi - ylo) * (10 - h - 10) / (yhi - ylo) + h;
            spr->drawFloat(yhi, dp, 36, yp1, 1);
        }*/

        if (y1 > 0) 
        {
            spr->setTextColor(pcolor1, bcolor);
            double yp1 =  (y1 - ylo) * (10 - h - 10) / (yhi - ylo) + h;
            spr->drawFloat(y1, dp, 36, yp1, 1);
        }
        if (y2 > 0) 
        {
            spr->setTextColor(pcolor2, bcolor);
            double yp2 =  (y2 - ylo) * (10 - h - 10) / (yhi - ylo) + h;
            spr->drawFloat(y2, dp, 36, yp2, 1);
        }

        spr->pushSprite(gx - 40, gy - h);
        spr->deleteSprite();

        // this axis doesn't update as often?
        // spr->createSprite(w, 20);

        // draw x scale
        for (i = xlo; i <= xhi; i += xinc)
        {

            // compute the transform
            temp = (i - xlo) * (w) / (xhi - xlo) + gx;
            if (i == 0)
            {
                tft->drawLine(temp, gy, temp, gy - h, acolor);
                tft->setTextColor(acolor, bcolor);
                tft->setTextDatum(BC_DATUM);
                tft->drawString(ylabel, (int)temp, (int)(gy - h - 8), 2);
            }
            /*else
            {
                tft->drawLine(temp, gy, temp, gy - h, gcolor);
            }*/
            // draw the axis labels
            tft->setTextColor(tcolor, bcolor);
            tft->setTextDatum(TC_DATUM);
            // precision is default Arduino--this could really use some format control
            tft->drawFloat(i, 0, temp, gy + 7, 1);
        }

        // now draw the graph labels
        tft->setTextColor(tcolor, bcolor);
        tft->drawString(title, (int)(gx + w / 2), (int)(gy - h - 30), 4);
    }

    // the coordinates are now drawn, plot the data
    // the entire plotting code are these few lines...
    // recall that ox and oy1 are initialized above
    // x =  (x - xlo) * ( w) / (xhi - xlo) + gx;
    // y =  (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
    // tft.drawLine(ox, oy1, x, y, pcolor);
    // it's up to you but drawing 2 more lines to give the graph some thickness
    // tft.drawLine(ox, oy1 + 1, x, y + 1, pcolor);
    // tft.drawLine(ox, oy1 - 1, x, y - 1, pcolor);
    // ox = x;
    // oy1 = y;
}

void AerChart::Trace(TFT_eSprite *spr, double x, double y1, double y2, byte dp,
                     double gx, double gy,
                     double w, double h,
                     double xlo, double xhi, double xinc,
                     double ylo, double yhi, double yinc,
                     const char *title, const char *xlabel, const char *ylabel,
                     bool &update1, unsigned int color, unsigned int bcolor)
{
    // double ydiv, xdiv;
    double i;
    double temp;
    // int rot, newrot;

    spr->setTextSize(1);

    unsigned int gcolor = DKBLUE;      // gcolor = graph grid color
    unsigned int acolor = RED;         // acolor = main axes and label color
    unsigned int pcolor1 = color;      // pcolor = color of your plotted data
    unsigned int pcolor2 = TFT_YELLOW; // pcolor = color of your plotted data
    unsigned int tcolor = WHITE;       // tcolor = text color
    // unsigned int bcolor = BLACK;  // bcolor = background color

    // initialize old x and old y in order to draw the first point of the graph
    // but save the transformed value
    // note my transform funcition is the same as the map function, except the map uses long and we need doubles
    if (update1)
    {
        update1 = false;

        ox = (x - xlo) * (w) / (xhi - xlo) + gx;
        oy1 = (y1 - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
        oy2 = (y2 - ylo) * (gy - h - gy) / (yhi - ylo) + gy;

        if ((ox < gx) || (ox > gx + w))
        {
            update1 = true;
            return;
        }
        if ((oy1 < gy - h) || (oy1 > gy))
        {
            // update1 = true;
            // return;
        }
        /*if ((oy2 < gy - h) || (oy2 > gy))
        {
            // update1 = true;
            return;
        }*/

        /// spr->setTextDatum(MR_DATUM);

        // draw y scale
        for (i = ylo; i <= yhi; i += yinc)
        {
            // compute the transform
            temp = (i - ylo) * (gy - h - gy) / (yhi - ylo) + gy;

            if (i == 0)
            {
                /// spr->setTextColor(acolor, bcolor);
                /// spr->drawString(xlabel, (int)(gx + w), (int)temp, 2);
            }
            else
            {
                spr->drawLine(gx, temp, gx + w, temp, gcolor);
            }
            // draw the axis labels
            /// spr->setTextColor(tcolor, bcolor);
            // precision is default Arduino--this could really use some format control
            /// spr->drawFloat(i, dp, gx - 4, temp, 1);
        }

        // draw x scale
        for (i = xlo; i <= xhi; i += xinc)
        {

            // compute the transform
            temp = (i - xlo) * (w) / (xhi - xlo) + gx;
            if (i == 0)
            {
                /// spr->setTextColor(acolor, bcolor);
                /// spr->setTextDatum(BC_DATUM);
                /// spr->drawString(ylabel, (int)temp, (int)(gy - h - 8), 2);
            }
            else
            {
                spr->drawLine(temp, gy, temp, gy - h, gcolor);
            }

            // draw the axis labels
            /// spr->setTextColor(tcolor, bcolor);
            /// spr->setTextDatum(TC_DATUM);
            // precision is default Arduino--this could really use some format control
            /// spr->drawFloat(i, dp, temp, gy + 7, 1);
        }

        // now draw the graph labels
        /// spr->setTextColor(tcolor, bcolor);
        /// spr->drawString(title, (int)(gx + w / 2), (int)(gy - h - 30), 4);
    }

    // the coordinates are now drawn, plot the data
    // the entire plotting code are these few lines...
    // recall that ox and oy1 are initialized above
    x = (x - xlo) * (w) / (xhi - xlo) + gx;
    y1 = (y1 - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
    y2 = (y2 - ylo) * (gy - h - gy) / (yhi - ylo) + gy;

    if ((x < gx) || (x > gx + w))
    {
        update1 = true;
        return;
    }
    // draw y1
    if ((y1 > gy - h) && (y1 < gy))
    {
        spr->drawLine(ox, oy1, x, y1, pcolor1);
        oy1 = y1;
    }
    // draw y2
    if ((y2 > gy - h) && (y2 < gy))
    {
        spr->drawLine(ox, oy2, x, y2, pcolor2);
        oy2 = y2;
    }
    // it's up to you but drawing 2 more lines to give the graph some thickness
    // spr->drawLine(ox, oy1 + 1, x, y + 1, pcolor);
    // spr->drawLine(ox, oy1 - 1, x, y - 1, pcolor);
    
    ox = x;
}

void AerChart::Trace(TFT_eSprite *spr, double x, double y1, double y2, double y3, double y4,
                     byte dp,
                     double gx, double gy,
                     double w, double h,
                     double xlo, double xhi, double xinc,
                     double ylo, double yhi, double yinc,
                     const char *title, const char *xlabel, const char *ylabel,
                     bool &update1, unsigned int bcolor)
{
    // double ydiv, xdiv;
    double i;
    double temp;
    // int rot, newrot;

    spr->setTextSize(1);

    unsigned int gcolor = DKBLUE;                 // gcolor = graph grid color
    unsigned int acolor = RED;                    // acolor = main axes and label color
    unsigned int pcolor1 = color565(8, 255, 32);  // pcolor = color of your plotted data
    unsigned int pcolor2 = color565(255, 255, 0); // pcolor = color of your plotted data
    unsigned int pcolor3 = color565(196, 8, 255); // pcolor = color of your plotted data
    unsigned int pcolor4 = color565(255, 255, 0); // pcolor = color of your plotted data
    unsigned int tcolor = WHITE;                  // tcolor = text color
    // unsigned int bcolor = BLACK;  // bcolor = background color

    // initialize old x and old y in order to draw the first point of the graph
    // but save the transformed value
    // note my transform funcition is the same as the map function, except the map uses long and we need doubles
    if (update1)
    {
        update1 = false;

        ox = (x - xlo) * (w) / (xhi - xlo) + gx;
        oy1 = (y1 - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
        oy2 = (y2 - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
        oy3 = (y3 - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
        oy4 = (y4 - ylo) * (gy - h - gy) / (yhi - ylo) + gy;

        if ((ox < gx) || (ox > gx + w))
        {
            update1 = true;
            return;
        }

        // draw y scale
        for (i = ylo; i <= yhi; i += yinc)
        {
            // compute the transform
            temp = (i - ylo) * (gy - h - gy) / (yhi - ylo) + gy;

            if (i == 0)
            {
                /// spr->setTextColor(acolor, bcolor);
                /// spr->drawString(xlabel, (int)(gx + w), (int)temp, 2);
            }
            else
            {
                spr->drawLine(gx, temp, gx + w, temp, gcolor);
            }
            // draw the axis labels
            /// spr->setTextColor(tcolor, bcolor);
            // precision is default Arduino--this could really use some format control
            /// spr->drawFloat(i, dp, gx - 4, temp, 1);
        }

        // draw x scale
        for (i = xlo; i <= xhi; i += xinc)
        {

            // compute the transform
            temp = (i - xlo) * (w) / (xhi - xlo) + gx;
            if (i == 0)
            {
                /// spr->setTextColor(acolor, bcolor);
                /// spr->setTextDatum(BC_DATUM);
                /// spr->drawString(ylabel, (int)temp, (int)(gy - h - 8), 2);
            }
            else
            {
                spr->drawLine(temp, gy, temp, gy - h, gcolor);
            }

            // draw the axis labels
            /// spr->setTextColor(tcolor, bcolor);
            /// spr->setTextDatum(TC_DATUM);
            // precision is default Arduino--this could really use some format control
            /// spr->drawFloat(i, dp, temp, gy + 7, 1);
        }

        // now draw the graph labels
        /// spr->setTextColor(tcolor, bcolor);
        /// spr->drawString(title, (int)(gx + w / 2), (int)(gy - h - 30), 4);
    }

    // the coordinates are now drawn, plot the data
    // the entire plotting code are these few lines...
    // recall that ox and oy1 are initialized above
    x = (x - xlo) * (w) / (xhi - xlo) + gx;
    y1 = (y1 - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
    y2 = (y2 - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
    y3 = (y3 - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
    y4 = (y4 - ylo) * (gy - h - gy) / (yhi - ylo) + gy;

    if ((x < gx) || (x > gx + w))
    {
        update1 = true;
        return;
    }

    // draw y1
    if ((y1 > gy - h) && (y1 < gy))
    {
        spr->drawLine(ox, oy1, x, y1, pcolor1);
        oy1 = y1;
    }

    // draw y2
    if ((y2 > gy - h) && (y2 < gy))
    {
        spr->drawLine(ox, oy2, x, y2, pcolor2);
        oy2 = y2;
    }

    // draw y3
    if ((y3 > gy - h) && (y3 < gy))
    {
        spr->drawLine(ox, oy3, x, y3, pcolor3);
        oy3 = y3;
    }

    // draw y4
    if ((y4 > gy - h) && (y4 < gy))
    {
        spr->drawLine(ox, oy4, x, y4, pcolor4);
        oy4 = y4;
    }

    // set old x to current
    ox = x;
}