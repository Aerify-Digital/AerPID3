#ifndef TEMPSTOR_H
#define TEMPSTOR_H

#include "flash.h"

class TempStor
{
public:
  void init(Flash *flash);

  void load1();
  void save1();

  void load2();
  void save2();

  bool setupDefaults(uint8_t elementIndex);

  void setTemp(double temp);
  double getTemp();

  void setMaxTemp(double temp);
  double getMaxTemp();

#if AERPID_COUNT == 2
  void setTemp2(double temp);
  double getTemp2();

  void setMaxTemp2(double temp);
  double getMaxTemp2();
#endif

private:
  Flash *flash;
  double set_Temp = 0;
  double set_Temp2 = 0;
  double max_Temp = 450;
  double max_Temp2 = 450;

  bool loaded = false;

  union DoubleByte
  {
    byte array[sizeof(double)];
    double num;
  };
};

extern TempStor tempStor;

#endif