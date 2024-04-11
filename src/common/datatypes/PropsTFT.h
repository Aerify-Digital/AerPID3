#ifndef _PROPS_TFT_H_
#define _PROPS_TFT_H_

#include <TFT_eSPI.h>
#include "PropsMenu.h"

typedef struct PropsTFT
{
    TFT_eSPI *tft;
    PropsMenu *menu;

} PropsTFT;

#endif