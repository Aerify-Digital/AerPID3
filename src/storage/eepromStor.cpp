#include "eepromStor.h"

/**
 * @brief Initialize the EEPROM Storage Class
 *
 * @param eeprom
 * @param PIN
 */
void EepromStorage::init(extEEPROM *eeprom, uint8_t PIN)
{
  EepromStorage::ee = eeprom;
  EepromStorage::EEP_PIN_EN = PIN;
  EepromStorage::writeDisable();
  Serial.println("EEPROM Storage Initialized!");
}

/**
 * @brief Gets data from the buffer
 *
 * @return char*
 */
char *EepromStorage::getData()
{
  return EepromStorage::data;
}

/**
 * @brief Writes data directly to the buffer
 *
 * @param data
 */
void EepromStorage::setData(char *data)
{
  for (int i = 0; i < sizeof(data); i++)
  {
    if (i > 128)
    {
      break;
    }
    EepromStorage::data[i] = data[i];
  }
}

// =========================================================
// EEPROM Support Functions
// =========================================================

/**
 * @brief Read from the EEPROM into the storage buffer and return the buffer
 *
 * @param o offset
 * @param l length
 * @return char* buffer
 */
char *EepromStorage::rd(uint16_t o, uint8_t l)
{
  for (int i = 0; i < l; i++)
  {
    EepromStorage::data[i] = EepromStorage::rdb(o + i);
  }
  return EepromStorage::data;
}

/**
 * @brief Read from the EEPROM into the storage buffer
 *
 * @param o offset
 * @param c length
 */
void EepromStorage::rb(uint16_t o, int c)
{
  int k = 0;
  while (k < c)
  {
    EepromStorage::data[k] = EepromStorage::rdb(o + k);
    k++;
  }
}

/**
 * @brief Read a single byte from the EEPROM and return it
 *
 * @param o offset
 * @return char
 */
char EepromStorage::rdb(uint16_t o)
{
  return EepromStorage::ee->read(o);
}

/**
 * @brief Write double to EEPROM
 *
 * @param o offset
 * @param d double
 */
void EepromStorage::wr(uint16_t o, const double d)
{
  EepromStorage::writeEnable();
  dtostrf(d, 8, 3, EepromStorage::data);
  EepromStorage::ee->write(o, (uint8_t *)EepromStorage::data, sizeof(EepromStorage::data));
  EepromStorage::writeDisable();
}

/**
 * @brief Write integer to EEPROM
 *
 * @param o offset
 * @param c int
 */
void EepromStorage::wrb(uint16_t o, uint8_t c)
{
  EepromStorage::writeEnable();
  EepromStorage::ee->write(o, c);
  EepromStorage::writeDisable();
}

/**
 * @brief Write a buffer directly to the EEPROM at a given offset and length
 *
 * @param offset
 * @param buf
 * @param length
 * @return true
 * @return false
 */
bool EepromStorage::write(int offset, byte *buf, int length)
{
  if (EepromStorage::ee->write(offset, buf, length) == EEPROM_ADDR_ERR)
  {
    return false;
  }
  return true;
}

/**
 * @brief Write a byte directly to the EEPROM at a given index
 *
 * @param index
 * @param buf
 * @return true
 * @return false
 */
bool EepromStorage::write(int index, byte buf)
{
  if (EepromStorage::ee->write(index, buf) == EEPROM_ADDR_ERR)
  {
    return false;
  }
  return true;
}

/**
 * @brief Toggle write enable Pin to Enable
 *
 */
void EepromStorage::writeEnable()
{
  digitalWrite(EEP_PIN_EN, LOW);
  delayMicroseconds(500);
}

/**
 * @brief Toggle write enable Pin to Disable
 *
 */
void EepromStorage::writeDisable()
{
  delayMicroseconds(200);
  digitalWrite(EEP_PIN_EN, HIGH);
}

// =============================================================
// =============================================================
// EEPROM
// =============================================================
void EepromStorage::save_kP(double kP)
{
  EepromStorage::wr(10, kP);
}

double EepromStorage::load_kP()
{
  double kP = String(EepromStorage::rd(10, 8)).toDouble();
  return kP;
}

void EepromStorage::save_kI(double kI)
{
  Serial.print("> I Value Save: ");
  Serial.println(kI, 4);
  EepromStorage::wr(20, kI);
}

double EepromStorage::load_kI()
{
  // double kI = String(EepromStorage::rd(20, 10)).toDouble();
  Serial.println("> I Value Raw: ");
  Serial.println(EepromStorage::rd(20, 10));
  double kI = atof(EepromStorage::rd(20, 10));
  return kI;
}

void EepromStorage::save_kD(double kD)
{
  EepromStorage::wr(30, kD);
}

double EepromStorage::load_kD()
{
  double kD = String(EepromStorage::rd(30, 8)).toDouble();
  return kD;
}

void EepromStorage::saveUUIDflag(uint8_t f)
{

  EepromStorage::write(49, 1);
}

uint8_t EepromStorage::loadUUIDflag()
{
  return EepromStorage::rdb(49);
}

void EepromStorage::saveUUID(char *uuid)
{
  EepromStorage::write(50, (byte *)uuid, 37);
}

char *EepromStorage::loadUUID()
{
  return EepromStorage::rd(50, 37);
}

void EepromStorage::saveKeyAES(char *key)
{
  EepromStorage::write(96, (byte *)key, 16);
}

char *EepromStorage::loadKeyAES()
{
  return EepromStorage::rd(96, 16);
}

EepromStorage eepromStor;