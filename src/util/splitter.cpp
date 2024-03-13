#include "splitter.h"

bool SplitterClass::split(String input)
{
  SplitterClass::dispose();
  SplitterClass::input = input;
  for (int i = 0; i < input.length(); i++)
  {
    if (input.substring(i, i + 1) == del)
    {
      SplitterClass::siz++;
    }
  }
  if (SplitterClass::size() > 1)
  {
    SplitterClass::buildContent();
  }
  return SplitterClass::size() > 1;
}

bool SplitterClass::split(String input, String del)
{
  SplitterClass::dispose();
  SplitterClass::input = input;
  SplitterClass::del = del;
  for (int i = 0; i < input.length(); i++)
  {
    if (input.substring(i, i + 1) == del)
    {
      SplitterClass::siz++;
    }
  }
  if (SplitterClass::size() > 1)
  {
    SplitterClass::buildContent();
  }
  return SplitterClass::size() > 1;
}

int SplitterClass::size()
{
  return SplitterClass::siz;
}

String *SplitterClass::contents()
{
  return SplitterClass::content;
}

void SplitterClass::buildContent()
{
  String input = SplitterClass::input;
  int s = SplitterClass::size();
  int i = 0;
  while (true)
  {
    if (i >= s)
    {
      break;
    }
    String firstVal, secondVal;
    SplitterClass::splitTwo(input, firstVal, secondVal);
    SplitterClass::content[i++] = firstVal;
    if (SplitterClass::contains(secondVal) == false)
    {
      SplitterClass::content[i++] = secondVal;
      break;
    }
    input = secondVal;
  }
  SplitterClass::disposed = false;
}

void SplitterClass::splitTwo(String input, String &firstVal, String &secondVal)
{
  for (int i = 0; i < input.length(); i++)
  {
    if (input.substring(i, i + 1) == del)
    {
      firstVal = input.substring(0, i);
      secondVal = input.substring(i + 1);
      break;
    }
  }
}

bool SplitterClass::contains(String input)
{
  for (int i = 0; i < input.length(); i++)
  {
    if (input.substring(i, i + 1) == del)
    {
      return true;
    }
  }
  return false;
}

void SplitterClass::dispose()
{
  SplitterClass::siz = 1;
  SplitterClass::input = "";
  SplitterClass::content[16];
  SplitterClass::disposed = true;
}

// Instance..
SplitterClass Splitt;
