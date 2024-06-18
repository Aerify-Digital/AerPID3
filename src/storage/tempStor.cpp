#include "tempStor.h"

void TempStor::init(Flash *flash)
{
  TempStor::flash = flash;

  load1();
#if AERPID_COUNT == 2
  load2();
#endif
}

void TempStor::load1()
{
  const char *filename = "temp1.dat";
  uint8_t len = 64;

  char data[len];
  Serial.println("[TempStor]>Setup: Loading Temp settings from Flash.");
  if (!TempStor::flash->openFile(filename, (char *)data, len))
  {
    Serial.println("[TempStor]>Error: Temp File not found!");
    setupDefaults(0);
    return;
  }
  // Serial.println((char *)data);

  TempStor::loaded = true;

  DoubleByte st;
  DoubleByte mt;

  for (int i = 0; i < len; i++)
  {
    if (i < 8)
    {
      st.array[i] = data[i];
    }
    else if (i < 16)
    {
      mt.array[i - 8] = data[i];
    }
  }

  TempStor::set_Temp = st.num;
  TempStor::max_Temp = mt.num;
}

void TempStor::load2()
{
  const char *filename = "temp2.dat";
  uint8_t len = 64;

  char data[len];
  Serial.println("[TempStor]>Setup: Loading Temp settings from Flash.");
  if (!TempStor::flash->openFile(filename, (char *)data, len))
  {
    Serial.println("[TempStor]>Error: Temp File not found!");
    setupDefaults(1);
    return;
  }
  // Serial.println((char *)data);

  TempStor::loaded = true;

  DoubleByte st;
  DoubleByte mt;

  for (int i = 0; i < len; i++)
  {
    if (i < 8)
    {
      st.array[i] = data[i];
    }
    else if (i < 16)
    {
      mt.array[i - 8] = data[i];
    }
  }

  TempStor::set_Temp2 = st.num;
  TempStor::max_Temp2 = mt.num;
}

void TempStor::save1()
{
  const char *filename = "temp1.dat";
  uint8_t len = 64;

  char data[len];

  DoubleByte st;
  DoubleByte mt;

  st.num = set_Temp;
  mt.num = max_Temp;

  for (int i = 0; i < len; i++)
  {
    if (i < 8)
    {
      data[i] = st.array[i];
    }
    else if (i < 16)
    {
      data[i] = mt.array[i + 8];
    }
  }

  // Serial.println((char *)data);
  TempStor::flash->saveFile(filename, data, len);
}

void TempStor::save2()
{
  const char *filename = "temp2.dat";
  uint8_t len = 16;

  char data[len];

  DoubleByte st;
  DoubleByte mt;

  st.num = set_Temp2;
  mt.num = max_Temp2;

  for (int i = 0; i < len; i++)
  {
    if (i < 8)
    {
      data[i] = st.array[i];
    }
    else if (i < 16)
    {
      data[i] = mt.array[i + 8];
    }
  }

  // Serial.println((char *)data);
  TempStor::flash->saveFile(filename, data, len);
}

bool TempStor::setupDefaults(uint8_t ei)
{
  Serial.println("[TempStor] > Applying Defaults!");
  switch (ei)
  {
  case 0:
    TempStor::set_Temp = 250;
    TempStor::max_Temp = 850;
    save1();
    break;
  case 1:
    TempStor::set_Temp2 = 250;
    TempStor::max_Temp2 = 850;
    save2();
    break;
  default:
    break;
  }
  TempStor::loaded = true;

  return false;
}

void TempStor::setTemp(double temp)
{
  TempStor::set_Temp = temp;
}

double TempStor::getTemp()
{
  return TempStor::set_Temp;
}

void TempStor::setMaxTemp(double temp)
{
  TempStor::max_Temp = temp;
}

double TempStor::getMaxTemp()
{
  return TempStor::max_Temp;
}

#if AERPID_COUNT == 2
void TempStor::setTemp2(double temp)
{
  TempStor::set_Temp2 = temp;
}

double TempStor::getTemp2()
{
  return TempStor::set_Temp2;
}

void TempStor::setMaxTemp2(double temp)
{
  TempStor::max_Temp2 = temp;
}

double TempStor::getMaxTemp2()
{
  return TempStor::max_Temp2;
}
#endif

TempStor tempStor;