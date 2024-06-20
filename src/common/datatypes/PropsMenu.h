#ifndef _GUI_MENU_H
#define _GUI_MENU_H

#include <stdbool.h>
#include <stdint.h>
#include <iostream>
#include <string.h>

typedef struct PropsMenu
{
    uint16_t menuIndex;
    uint16_t menuLevelVal;
    uint16_t menuLevelId;
    int16_t menuItemId;
    bool menuUpdate;
    bool menuChange;
    std::string menuItemSelStr;
    uint8_t menuItemSelIpAddrIndex;
    uint8_t menuItemIpAddr[4];
} PropsMenu;

#endif