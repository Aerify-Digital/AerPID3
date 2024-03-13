#include "uuid.h"

#include "HardwareSerial.h"

void UUIDclass::printHex(uint8_t number, char *hexChars)
{
  int topDigit = number >> 4;
  int bottomDigit = number & 0x0f;
  // Print high hex digit
  hexChars[0] = "0123456789ABCDEF"[topDigit];
  // Serial.print( hexChars[0] );
  //  Low hex digit
  hexChars[1] = "0123456789ABCDEF"[bottomDigit];
  // Serial.print( hexChars[1] );
}

void UUIDclass::printUuid(uint8_t *uuidNumber)
{
  int j = 0;
  for (int i = 0; i < 16; i++)
  {
    if (j > 32 + 4)
    {
      break;
    }
    if (i == 4 || i == 6 || i == 8 || i == 10)
    {
      uuidValue[j++] = '-';
      // Serial.print("-");
    }
    char hexx[2];
    printHex(uuidNumber[i], hexx);
    uuidValue[j++] = hexx[0];
    uuidValue[j++] = hexx[1];
  }
}

char *UUIDclass::generate()
{
  // Serial.println("Generate New UUID...");
  //  Generate a new UUID
  TrueRandom.uuid(uuidNumber);
  // Serial.print("The UUID number is ");
  UUIDclass::printUuid(uuidNumber);
  // Serial.println();
  return uuidValue;
}

bool UUIDclass::validate(char *uuidChars)
{
  uint8_t bytes[16];
  UUIDclass::convertArrayToHex(uuidChars, bytes);
  if (bytes[3] == 222 && bytes[4] == 0 && bytes[5] == 1)
  {
    for (int i = 0; i < 16; i++)
    {
      uuidNumber[i] = bytes[i];
    }
    return true;
  }
  return false;
}

void UUIDclass::convertArrayToHex(char *hexCharArray, uint8_t *hexByteArray)
{
  int j = 0;
  for (char i = 0; i < 32 + 4; i++)
  {
    if (hexCharArray[i] == '-')
    {
      continue;
    }
    if (j >= 16)
    {
      break;
    }
    uint8_t extract;
    char a = hexCharArray[i];
    char b = hexCharArray[++i];
    extract = convertCharToHex(a) << 4 | convertCharToHex(b);
    hexByteArray[j++] = extract;
    // Serial.print(hexByteArray[j - 1]);
    // Serial.print(" ");
  }
  // Serial.println("");
}

char UUIDclass::convertCharToHex(char ch)
{
  char returnType;
  switch (ch)
  {
  case '0':
    returnType = 0;
    break;
  case '1':
    returnType = 1;
    break;
  case '2':
    returnType = 2;
    break;
  case '3':
    returnType = 3;
    break;
  case '4':
    returnType = 4;
    break;
  case '5':
    returnType = 5;
    break;
  case '6':
    returnType = 6;
    break;
  case '7':
    returnType = 7;
    break;
  case '8':
    returnType = 8;
    break;
  case '9':
    returnType = 9;
    break;
  case 'A':
    returnType = 10;
    break;
  case 'B':
    returnType = 11;
    break;
  case 'C':
    returnType = 12;
    break;
  case 'D':
    returnType = 13;
    break;
  case 'E':
    returnType = 14;
    break;
  case 'F':
    returnType = 15;
    break;
  case '-':
    returnType = 0;
    break;
  default:
    returnType = 0;
    break;
  }
  return returnType;
}

// External Reference
UUIDclass UUID;
