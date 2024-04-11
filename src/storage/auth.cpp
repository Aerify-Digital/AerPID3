#include "auth.h"

bool Auth::init(Flash *flash)
{
  Serial.println("Loading Auth..");
  Auth::flsh1 = flash;
  Auth::loadFromFlash();
  Serial.println("Auth Loaded!");
  return true;
}

void Auth::loadFromFlash()
{
  char pass[64];
  bool authed = Auth::flsh1->openAuthFile(pass);
  if (!authed)
  {
    Auth::authed = false;
    return;
  }
  if (String(pass).equals("de4037d83f53e715500a9cc6b9b1c3da8c0e363cfaaf67e3756689cdc4284b6f"))
  {
    Auth::authed = false;
    return;
  }
  if (String(pass).equals("") || pass == "\0")
  {
    Auth::authed = false;
    return;
  }
  Auth::locked = true;
  Auth::SetPassphrase(pass, false);
}

void Auth::saveToFlash()
{
  Auth::flsh1->saveAuthFile(Auth::GetPassphrase());
}

bool Auth::TestPassphrase(const char *pass)
{
  if (String(pass).equals(String(Auth::GetPassphrase())))
  {
    Auth::loggedIn = true;
    Auth::valid = Auth::isAuthed();
    return true;
  }
  return false;
}

char *Auth::GetPassphrase()
{
  return Auth::passphrase;
}

void Auth::SetPassphrase(char *pass, bool save)
{
  for (int i = 0; i < 64; i++)
  {
    Auth::passphrase[i] = pass[i];
  }
  Auth::authed = true;
  if (save)
  {
    Auth::saveToFlash();
  }
}

bool Auth::isAuthed()
{
  return Auth::authed;
}

bool Auth::isLoggIn()
{
  return Auth::loggedIn;
}

void Auth::logout()
{
  Auth::loggedIn = false;
  Auth::validPIN = false;
}

bool Auth::isValid()
{
  return Auth::valid;
}

bool Auth::isValidPIN()
{
  return Auth::validPIN;
}

void Auth::SetValidPIN()
{
  Auth::validPIN = true;
}

void Auth::InvalidatePIN()
{
  Auth::validPIN = false;
}

void Auth::setPin64(char *pin)
{
  for (int i = 0; i < 6; i++)
  {
    Auth::pin64[i] = pin[i];
  }
}

char *Auth::getPin64()
{
  return Auth::pin64;
}

bool Auth::isLocked()
{
  return Auth::locked;
}

void Auth::reset_f()
{
  Auth::locked = false;
  Auth::authed = false;
  flsh1->deleteKeys();
}

// Authentication
Auth auth;
