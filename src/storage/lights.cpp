#include "lights.h"

void LightsStor::init(Flash *flash)
{
    this->flash = flash;
    load();
}

void LightsStor::load()
{
    Serial.println("[LightsStor]>Setup: Loading LED Settings from Flash.");
    // String s;
    // File ledFile = SPIFFS.open("/lights.txt");
    char data[128];
    if (!LightsStor::flash->openLedFile((char *)data))
    {
        Serial.println("[LightsStor]>Error: LED File not found!");
        LightsStor::setup();
        return;
    }
    // Serial.println((char*) data);
    char *tok = strtok((char *)data, "\n");
    LightsStor::enabled = (strcmp(tok, "true") == 0 ? true : false);
    char *tok1 = strtok(NULL, "\n");
    LightsStor::led_status = (strcmp(tok1, "true") == 0 ? true : false);
    char *tok2 = strtok(NULL, "\n");
    LightsStor::led_ambient = (strcmp(tok2, "true") == 0 ? true : false);
    char *tok3 = strtok(NULL, "\n");
    LightsStor::led_am_pulse = (strcmp(tok3, "true") == 0 ? true : false);
    char *tok4 = strtok(NULL, "\n");
    LightsStor::led_am_blink = (strcmp(tok4, "true") == 0 ? true : false);
    char *tok5 = strtok(NULL, "\n");
    LightsStor::led_preset = (strcmp(tok5, "true") == 0 ? true : false);
    char *tok6 = strtok(NULL, "\n");
    LightsStor::led_pre_rain = (strcmp(tok6, "true") == 0 ? true : false);
    char *tok7 = strtok(NULL, "\n");
    LightsStor::led_pre_rain2 = (strcmp(tok7, "true") == 0 ? true : false);
    char *tok8 = strtok(NULL, "\n");
    LightsStor::led_pre_rain3 = (strcmp(tok8, "true") == 0 ? true : false);
    char *tok9 = strtok(NULL, "\n");
    LightsStor::led_pre_pulse = (strcmp(tok9, "true") == 0 ? true : false);
    char *tok10 = strtok(NULL, "\n");
    LightsStor::led_pre_shift = (strcmp(tok10, "true") == 0 ? true : false);
    char *tok11 = strtok(NULL, "\n");
    LightsStor::LED_R = String(tok11).toInt();
    char *tok12 = strtok(NULL, "\n");
    LightsStor::LED_G = String(tok12).toInt();
    char *tok13 = strtok(NULL, "\n");
    LightsStor::LED_B = String(tok13).toInt();
    char *tok14 = strtok(NULL, "\n");
    LightsStor::LED_Bright = String(tok14).toInt();

    LightsStor::loaded = true;
    Serial.println("[LightsStor]>Setup: LED Settings loaded.");
}

void LightsStor::save()
{
    char data[128];
    String saveString = String((LightsStor::enabled ? "true" : "false")) + '\n';
    saveString += String((LightsStor::led_status ? "true" : "false")) + '\n';
    saveString += String((LightsStor::led_ambient ? "true" : "false")) + '\n';
    saveString += String((LightsStor::led_am_pulse ? "true" : "false")) + '\n';
    saveString += String((LightsStor::led_am_blink ? "true" : "false")) + '\n';
    saveString += String((LightsStor::led_preset ? "true" : "false")) + '\n';
    saveString += String((LightsStor::led_pre_rain ? "true" : "false")) + '\n';
    saveString += String((LightsStor::led_pre_rain2 ? "true" : "false")) + '\n';
    saveString += String((LightsStor::led_pre_rain3 ? "true" : "false")) + '\n';
    saveString += String((LightsStor::led_pre_pulse ? "true" : "false")) + '\n';
    saveString += String((LightsStor::led_pre_shift ? "true" : "false")) + '\n';
    saveString += String(LightsStor::LED_R) + '\n';
    saveString += String(LightsStor::LED_G) + '\n';
    saveString += String(LightsStor::LED_B) + '\n';
    saveString += String(LightsStor::LED_Bright) + '\n';
    strcpy(data, saveString.c_str());

    /*Serial.println((char *)data);*/
    LightsStor::flash->saveLedFile(data);
}

bool LightsStor::setup()
{
    save();
    return true;
}

bool LightsStor::isEnabled()
{
    return LightsStor::enabled;
}
void LightsStor::isEnabled(bool state)
{
    LightsStor::enabled = state;
}

bool LightsStor::statusEnabled()
{
    return LightsStor::led_status;
}
void LightsStor::statusEnabled(bool state)
{
    LightsStor::led_status = state;
}

bool LightsStor::ambientEnabled()
{
    return LightsStor::led_ambient;
}
void LightsStor::ambientEnabled(bool state)
{
    LightsStor::led_ambient = state;
}

bool LightsStor::amPulseEnabled()
{
    return LightsStor::led_am_pulse;
}
void LightsStor::amPulseEnabled(bool state)
{
    LightsStor::led_am_pulse = state;
}

bool LightsStor::amBlinkEnabled()
{
    return LightsStor::led_am_blink;
}
void LightsStor::amBlinkEnabled(bool state)
{
    LightsStor::led_am_blink = state;
}

bool LightsStor::presetsEnabled()
{
    return LightsStor::led_preset;
}
void LightsStor::presetsEnabled(bool state)
{
    LightsStor::led_preset = state;
}

bool LightsStor::preRainEnabled()
{
    return LightsStor::led_pre_rain;
}
void LightsStor::preRainEnabled(bool state)
{
    LightsStor::led_pre_rain = state;
}

bool LightsStor::preRain2Enabled()
{
    return LightsStor::led_pre_rain2;
}
void LightsStor::preRain2Enabled(bool state)
{
    LightsStor::led_pre_rain2 = state;
}

bool LightsStor::preRain3Enabled()
{
    return LightsStor::led_pre_rain3;
}
void LightsStor::preRain3Enabled(bool state)
{
    LightsStor::led_pre_rain3 = state;
}

bool LightsStor::prePulseEnabled()
{
    return LightsStor::led_pre_pulse;
}
void LightsStor::prePulseEnabled(bool state)
{
    LightsStor::led_pre_pulse = state;
}

bool LightsStor::preShiftEnabled()
{
    return LightsStor::led_pre_shift;
}
void LightsStor::preShiftEnabled(bool state)
{
    LightsStor::led_pre_shift = state;
}

uint8_t LightsStor::getRgbVal(char color)
{
    switch (color)
    {
    case 'r':
        return LightsStor::LED_R;
    case 'g':
        return LightsStor::LED_G;
    case 'b':
        return LightsStor::LED_B;
    default:
        return 0;
    }
}

String LightsStor::getRgbHex()
{
    if ((LightsStor::LED_R >= 0 && LightsStor::LED_R <= 255) && (LightsStor::LED_G >= 0 && LightsStor::LED_G <= 255) && (LightsStor::LED_B >= 0 && LightsStor::LED_B <= 255)) {
 
        String hexCode = "#";
        hexCode += decToHex(LightsStor::LED_R);
        hexCode += decToHex(LightsStor::LED_G);
        hexCode += decToHex(LightsStor::LED_B);
 
        return hexCode;
    }
 
    // The hex color code doesn't exist
    else
        return "-1";
    //return ((LightsStor::LED_R & 0xff) << 16) + ((LightsStor::LED_G & 0xff) << 8) + (LightsStor::LED_B & 0xff);
}

String LightsStor::decToHex(int n)
{
    // char array to store hexadecimal number
    char hexaDeciNum[2];
 
    // counter for hexadecimal number array
    int i = 0;
    while (n != 0) {
 
        // temporary variable to store remainder
        int temp = 0;
 
        // storing remainder in temp variable.
        temp = n % 16;
 
        // check if temp < 10
        if (temp < 10) {
            hexaDeciNum[i] = temp + 48;
            i++;
        }
        else {
            hexaDeciNum[i] = temp + 55;
            i++;
        }
 
        n = n / 16;
    }
 
    String hexCode = "";
    if (i == 2) {
        hexCode += hexaDeciNum[0];
        hexCode += hexaDeciNum[1];
    }
    else if (i == 1) {
        hexCode = "0";
        hexCode += hexaDeciNum[0];
    }
    else if (i == 0)
        hexCode = "00";
 
    // Return the equivalent
    // hexadecimal color code
    return hexCode;
}

void LightsStor::setRgbVal(char color, int8_t val)
{
    switch (color)
    {
    case 'r':
        LightsStor::LED_R = val;
    case 'g':
        LightsStor::LED_G = val;
    case 'b':
        LightsStor::LED_B = val;
    default:
        break;
    }
}
uint8_t LightsStor::getBright()
{
    return LightsStor::LED_Bright;
}
void LightsStor::setBright(int8_t bright)
{
    LightsStor::LED_Bright = bright;
}

uint8_t LightsStor::getMode() 
{
    //
    if (LightsStor::amBlinkEnabled()){
        return 1;
    }
    else if (LightsStor::amPulseEnabled()) {
        return 2;
    }
    else if (LightsStor::preRainEnabled()) {
        return 6;
    }
    else if (LightsStor::preRain2Enabled()) {
        return 4;
    }
    else if (LightsStor::preRain3Enabled()) {
        return 5;
    }
    else if (LightsStor::preShiftEnabled()) {
        return 7;
    }
    else if (LightsStor::prePulseEnabled()) {
        return 8;
    }
    else if (LightsStor::ambientEnabled()) {
        return 9;
    }
    else {
        return 9;
    }
}
void LightsStor::setMode(int8_t mode)
{
    LightsStor::ambientEnabled(false);
    LightsStor::amBlinkEnabled(false);
    LightsStor::amPulseEnabled(false);
    LightsStor::preRainEnabled(false);
    LightsStor::preRain2Enabled(false);
    LightsStor::preRain3Enabled(false);
    LightsStor::preShiftEnabled(false);
    LightsStor::prePulseEnabled(false);
    //LightsStor::statusEnabled(false);
    switch (mode)
    {
    case 1:
        LightsStor::amBlinkEnabled(true);
        break;
    case 2:
        LightsStor::amPulseEnabled(true);
        break;
    case 6:
        LightsStor::preRainEnabled(true);
        break;
    case 4:
        LightsStor::preRain2Enabled(true);
        break;
    case 5:
        LightsStor::preRain3Enabled(true);
        break;
    case 7:
        LightsStor::preShiftEnabled(true);
        break;
    case 8:
        LightsStor::prePulseEnabled(true);
        break;
    case 10:
        LightsStor::statusEnabled(true);
        break;
    case 9:
    default:
        LightsStor::ambientEnabled(true);
        break;
    }

}

LightsStor lightstor;