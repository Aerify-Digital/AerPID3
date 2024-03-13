#ifndef SPLITTER_H
#define SPLITTER_H

#include <Arduino.h>
#include <string.h>

class SplitterClass
{

public:
  bool split(String input);
  bool split(String input, String del);
  String *contents();
  int size();

  void dispose();

private:
  int siz = 1;
  String del = ",";
  String input;
  String content[16];
  bool disposed = false;

  void buildContent();

  void splitTwo(String input, String &firstVal, String &secondVal);
  bool contains(String input);
};

extern SplitterClass Splitt;

#endif
