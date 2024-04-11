#ifndef FAVSSTOR_H
#define FAVSSTOR_H

#include "flash.h"
#include "SPIFFS.h"
//#include "util/uuid.h"

class FavsStor
{
public:
  void init(Flash *flash);

  void load();
  void save();

  bool setup();

  void setFavTemp(double temp, int8_t fav);
  void softSetTemp(double temp, int8_t fav);
  void setFavName(char name[], int8_t fav);


  char *getName(int8_t fav);
  double getTemp(int8_t fav);

  //void loadNames();
  //void saveNames();

private:
  bool loaded = false;
  Flash *flash;

  char favName1[64];
  char favName2[64];
  char favName3[64];
  char favName4[64];
  double favTemp1;
  double favTemp2;
  double favTemp3;
  double favTemp4;
};

extern FavsStor favstor;

#endif