#include "favsStor.h"

void FavsStor::init(Flash *flash)
{
    FavsStor::flash = flash;

    load();
}

bool FavsStor::setup()
{
    // TODO: Make actual setup function to save the defaults.
    FavsStor::favName1[0] = 'F';
    FavsStor::favName1[1] = 'a';
    FavsStor::favName1[2] = 'v';
    FavsStor::favName1[3] = ' ';
    FavsStor::favName1[4] = '1';

    FavsStor::favName2[0] = 'F';
    FavsStor::favName2[1] = 'a';
    FavsStor::favName2[2] = 'v';
    FavsStor::favName2[3] = ' ';
    FavsStor::favName2[4] = '2';

    FavsStor::favName3[0] = 'F';
    FavsStor::favName3[1] = 'a';
    FavsStor::favName3[2] = 'v';
    FavsStor::favName3[3] = ' ';
    FavsStor::favName3[4] = '3';

    FavsStor::favName4[0] = 'F';
    FavsStor::favName4[1] = 'a';
    FavsStor::favName4[2] = 'v';
    FavsStor::favName4[3] = ' ';
    FavsStor::favName4[4] = '4';

    FavsStor::favTemp1 = 250;
    FavsStor::favTemp2 = 260;
    FavsStor::favTemp3 = 270;
    FavsStor::favTemp4 = 225;

    Serial.println("[FavsStor]>Setup: Default values loaded for Favorites. > Saving file...");
    save();
    return false;
}

void FavsStor::load()
{
    Serial.println("[FavsStor]>Setup: Attempting to load favs from Flash...");
    char data[289];
    if (!FavsStor::flash->openFavoritesFile((char *)data))
    {
        Serial.println("[FavsStor]>Error: Favorites File not found!");
        FavsStor::setup();
        return;
    }

    // Serial.println((char *)data);

    int j = 0;
    for (int i = 0; i < 64; i++)
    {
        if (data[j] != 0x7c)
        {
            FavsStor::favName1[i] = data[j];
        }
        j++;
    }
    j = 64;
    for (int i = 0; i < 64; i++)
    {
        if (data[j] != 0x7c)
        {
            FavsStor::favName2[i] = data[j];
        }
        j++;
    }
    j = 128;
    for (int i = 0; i < 64; i++)
    {
        if (data[j] != 0x7c)
        {
            FavsStor::favName3[i] = data[j];
        }
        j++;
    }
    j = 192;
    for (int i = 0; i < 64; i++)
    {
        if (data[j] != 0x7c)
        {
            FavsStor::favName4[i] = data[j];
        }
        j++;
    }

    /*union DoubleByte
    {
        byte array[8];
        double Num;
    } d;*/

    char read[8];
    j = 256;
    for (int i = 0; i < 8; i++)
    {
        if (data[j] != 0x7c)
        {
            read[i] = data[j];
        }
        j++;
    }
    FavsStor::favTemp1 = atof(read);
    memset(read, 0, 8);
    j = 264;
    for (int i = 0; i < 8; i++)
    {
        if (data[j] != 0x7c)
        {
            read[i] = data[j];
        }
        j++;
    }
    FavsStor::favTemp2 = atof(read);
    memset(read, 0, 8);
    j = 272;
    for (int i = 0; i < 8; i++)
    {
        if (data[j] != 0x7c)
        {
            read[i] = data[j];
        }
        j++;
    }
    FavsStor::favTemp3 = atof(read);
    memset(read, 0, 8);
    j = 280;
    for (int i = 0; i < 8; i++)
    {
        if (data[j] != 0x7c)
        {
            read[i] = data[j];
        }
        j++;
    }
    FavsStor::favTemp4 = atof(read);

    FavsStor::loaded = true;
    return;
}

void FavsStor::save()
{
    char data[289];

    int j = 0;
    for (int i = 0; i < 64; i++)
    {
        if (i < strlen(FavsStor::getName(1)))
        {
            data[j++] = FavsStor::getName(1)[i];
        }
        else
        {
            data[j++] = 0x7c;
        }
    }
    j = 64;
    for (int i = 0; i < 64; i++)
    {
        if (i < strlen(FavsStor::getName(2)))
        {
            data[j++] = FavsStor::getName(2)[i];
        }
        else
        {
            data[j++] = 0x7c;
        }
    }
    j = 128;
    for (int i = 0; i < 64; i++)
    {
        if (i < strlen(FavsStor::getName(3)))
        {
            data[j++] = FavsStor::getName(3)[i];
        }
        else
        {
            data[j++] = 0x7c;
        }
    }
    j = 192;
    for (int i = 0; i < 64; i++)
    {
        if (i < strlen(FavsStor::getName(4)))
        {
            data[j++] = FavsStor::getName(4)[i];
        }
        else
        {
            data[j++] = 0x7c;
        }
    }
    j = 256;
    String dAs = String(FavsStor::getTemp(1));
    char dAc[dAs.length() + 1];
    strcpy(dAc, dAs.c_str());
    for (int i = 0; i < 8; i++)
    {
        if (i < dAs.length())
        {
            data[j++] = dAc[i];
        }
        else
        {
            data[j++] = 0x7c;
        }
    }
    j = 264;
    dAs = String(FavsStor::getTemp(2));
    char dAc2[dAs.length() + 1];
    strcpy(dAc2, dAs.c_str());
    for (int i = 0; i < 8; i++)
    {
        if (i < dAs.length())
        {
            data[j++] = dAc2[i];
        }
        else
        {
            data[j++] = 0x7c;
        }
    }
    j = 272;
    dAs = String(FavsStor::getTemp(3));
    char dAc3[dAs.length() + 1];
    strcpy(dAc3, dAs.c_str());
    for (int i = 0; i < 8; i++)
    {
        if (i < dAs.length())
        {
            data[j++] = dAc3[i];
        }
        else
        {
            data[j++] = 0x7c;
        }
    }
    j = 280;
    dAs = String(FavsStor::getTemp(4));
    char dAc4[dAs.length() + 1];
    strcpy(dAc4, dAs.c_str());
    for (int i = 0; i < 8; i++)
    {
        if (i < dAs.length())
        {
            data[j++] = dAc4[i];
        }
        else
        {
            data[j++] = 0x7c;
        }
    }
    data[288] = '\0';

    // Serial.println((char *)data);
    FavsStor::flash->saveFavoritesFile(data);
    return;
}

void FavsStor::setFavName(char name[], int8_t fav)
{
    bool endStr = false;
    switch (fav)
    {
    case 1:
        for (int i = 0; i < 64; i++)
        {
            if (!name[i])
            {
                endStr = true;
            }
            if (!endStr)
            {
                FavsStor::favName1[i] = name[i];
            }
            else
            {
                FavsStor::favName1[i] = 0;
            }
        }
        break;
    case 2:
        for (int i = 0; i < 64; i++)
        {
            if (!name[i])
            {
                endStr = true;
            }
            if (!endStr)
            {
                FavsStor::favName2[i] = name[i];
            }
            else
            {
                FavsStor::favName1[i] = 0;
            }
        }
        break;
    case 3:
        for (int i = 0; i < 64; i++)
        {
            if (!name[i])
            {
                endStr = true;
            }
            if (!endStr)
            {
                FavsStor::favName3[i] = name[i];
            }
            else
            {
                FavsStor::favName1[i] = 0;
            }
        }
        break;
    case 4:
        for (int i = 0; i < 64; i++)
        {
            if (!name[i])
            {
                endStr = true;
            }
            if (!endStr)
            {
                FavsStor::favName4[i] = name[i];
            }
            else
            {
                FavsStor::favName1[i] = 0;
            }
        }
        break;
    default:
        Serial.println("[FavsStor]>SetFaveName failed: " + fav);
        return;
    }
    // Serial.println("[FavsStor]>SetFaveName name-saved: " + fav);
    // save();
    return;
}

void FavsStor::setFavTemp(double temp, int8_t fav)
{
    switch (fav)
    {
    case 1:
        FavsStor::favTemp1 = temp;
        break;
    case 2:
        FavsStor::favTemp2 = temp;
        break;
    case 3:
        FavsStor::favTemp3 = temp;
        break;
    case 4:
        FavsStor::favTemp4 = temp;
        break;
    default:
        Serial.println("[FavsStor]>SetFaveTemp failed: " + fav);
        return;
    }
    // Serial.println("[FavsStor]>SetFaveTemp temp-saved: " + fav);
    // save();
    return;
}

void FavsStor::softSetTemp(double temp, int8_t fav)
{
    switch (fav)
    {
    case 1:
        FavsStor::favTemp1 = temp;
        break;
    case 2:
        FavsStor::favTemp2 = temp;
        break;
    case 3:
        FavsStor::favTemp3 = temp;
        break;
    case 4:
        FavsStor::favTemp4 = temp;
        break;
    default:
        Serial.println("[FavsStor]>SetFaveTemp failed: " + fav);
        return;
    }
    return;
}

char *FavsStor::getName(int8_t fav)
{
    switch (fav)
    {
    case 1:
        return FavsStor::favName1;
    case 2:
        return FavsStor::favName2;
    case 3:
        return FavsStor::favName3;
    case 4:
        return FavsStor::favName4;
    default:
        return (char *)"-default-";
    }
    return (char *)"-failed-";
}

double FavsStor::getTemp(int8_t fav)
{
    switch (fav)
    {
    case 1:
        return FavsStor::favTemp1;
    case 2:
        return FavsStor::favTemp2;
    case 3:
        return FavsStor::favTemp3;
    case 4:
        return FavsStor::favTemp4;
    default:
        return 500;
    }
    return 500;
}

FavsStor favstor;