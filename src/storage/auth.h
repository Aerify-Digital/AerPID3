#ifndef AUTH_H
#define AUTH_H

#include "flash.h"

// @deprecated
class Auth
{
public:
  bool init(Flash *flash);

  char *GetPassphrase();
  void SetPassphrase(char *pass, bool save);
  bool TestPassphrase(const char *pass);

  bool isAuthed();
  bool isLoggIn();
  bool isValid();
  bool isLocked();
  bool isValidPIN();
  void SetValidPIN();
  void InvalidatePIN();

  void setPin64(char *pin);
  char *getPin64();

  void logout();
  void reset_f();

private:
  Flash *flsh1;
  bool authed = false;
  bool loggedIn = false;
  bool valid = false;
  bool locked = false;
  bool validPIN = false;

  char passphrase[64];
  char pin64[8];

  void loadFromFlash();
  void saveToFlash();
};

// Authentication
extern Auth auth;

#endif
