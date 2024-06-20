#include "serial_com.h"

void SerialCom::init(uint waitTime)
{
  Serial.begin(115200);
  SerialCom::loaded = 0;
  int c = 0;
  while (!Serial)
  {
    if (c > waitTime)
    {
      break;
    }
    c++;
    delayMicroseconds(1000 * 1000);
    // delay(1000);
  }
  Serial.println("\n\n> Serial Com Starting...");
}

void SerialCom::enPackFill(MessagePack *messagePack)
{
  NetworkVars *net = aerManager->getNetVars();
  messagePack->setIP(net->ip);
  messagePack->setHostname(net->hostname);
  messagePack->setSSID(net->ssid);

  AerPID *as = aerManager->getAerPID(0);
  messagePack->setUpTime(millis());
  messagePack->setBumpTemp(aerManager->getBump()->getTemp());
  messagePack->setBumpTime(aerManager->getBump()->getTime());
  messagePack->setAutoOffTime(as->AUTO_OFF_TIME);
  messagePack->setLedMode(0);           // TODO
  messagePack->setLedColor(21, 21, 21); // TODO
  messagePack->setLedBrightness(251);   // TODO

  FavsStor *favs = aerManager->getFavs();
  messagePack->setFavName(1, favs->getName(1));
  messagePack->setFavTemp(1, favs->getTemp(1));
  messagePack->setFavName(2, favs->getName(2));
  messagePack->setFavTemp(2, favs->getTemp(2));
  messagePack->setFavName(3, favs->getName(3));
  messagePack->setFavTemp(3, favs->getTemp(3));
  messagePack->setFavName(4, favs->getName(4));
  messagePack->setFavTemp(4, favs->getTemp(4));

  messagePack->setTempAdjAmt(as->TEMP_ADJ_AMOUNT);
  messagePack->setTemp(static_cast<uint16_t>(10.0 * as->MES_TEMP));
  messagePack->setAvgTemp(static_cast<uint16_t>(10.0 * as->AVG_TEMP));
  messagePack->setSetTemp(static_cast<uint16_t>(10.0 * as->SET_TEMP));
  messagePack->setPID(as->kP, as->kI, as->kD);

  SettingsStorage *sets = aerManager->getSettings();
  messagePack->setUnitType(sets->getThermalUnit()); // temp unit type

  // auto_off_enb, coil_enb, lights_enb, unused, wifi_enb, unused, bt_enb, unused
  byte bitmap = 0b00000000;
  bitWrite(bitmap, 7, aerManager->getAerPID(0)->AUTO_OFF_ENB);
  bitWrite(bitmap, 6, aerManager->getAerPID(0)->PID_ON);
  bitWrite(bitmap, 5, aerManager->getLights()->isEnabled());
  bitWrite(bitmap, 4, aerManager->getBump()->getEnabled(0)); // get bump enabled status
  bitWrite(bitmap, 3, aerManager->getComms()->getWifiEn());  // get status of wifi
  bitWrite(bitmap, 2, aerManager->getComms()->getSSIDSet()); // get ssid setup
  bitWrite(bitmap, 1, aerManager->getComms()->getBTEn());    // get status of bluetooth
  bitWrite(bitmap, 0, 0);                                    // unused
  messagePack->setBoleanBitmap(bitmap);
#if AERPID_COUNT == 2
  messagePack->setModel(2);
#else
  messagePack->setModel(1);
#endif
}

void SerialCom::begin(AerManager *aerm)
{
  SerialCom::aerManager = aerm;
  if (uart_is_driver_installed(0))
  {
    SerialCom::loaded = 1;
    Serial.println("> Serial Com Listener Started!");
  }
}

int SerialCom::isLoaded()
{
  return SerialCom::loaded;
}

int SerialCom::isConnected()
{
  return SerialCom::USB_CONNECTED;
}

void SerialCom::dispose()
{
  _hasMessage = false;
  free(decoded);
  bufferSize = 0;
  decoded = (char *)malloc(bufferSize);
}

char *SerialCom::getDecoded()
{
  return decoded;
}

bool SerialCom::hasMessage()
{
  return _hasMessage;
}

void SerialCom::serialReceiveEvent(HardwareSerial ser)
{
  int r = ser.read();
  if (r < 0)
  {
    return;
  }

  char c = r;
  if (c == START_BYTE)
  {
    // Start of message
    messageStarted = true;
    bytesRead = 0;
    bufferSize = 0; // Initial buffer size
    buffer = (char *)realloc(buffer, bufferSize);
    return;
  }

  if (!messageStarted)
  {
    // Ignore data until message starts
    return;
  }

  if (c == END_BYTE)
  {
    // End of message
    messageStarted = false;

    // Resize the buffer to the correct size
    buffer = (char *)realloc(buffer, bytesRead + 1);
    decoded = (char *)realloc(decoded, bytesRead + 1);

    // Process the message here
    buffer[bytesRead] = '\0';
    // ser.println(bytesRead);
    for (int i = 0; i < bytesRead; i++)
    {
      // Serial.print(buffer[i], 10);
      // ser.print(" ");
      decoded[i] = buffer[i];
    }
    // ser.println("");
    _hasMessage = true;

    // Free the buffer memory
    free(buffer);
    bufferSize = 0;
    buffer = (char *)malloc(bufferSize);
    return;
  }
  // Add data to buffer
  // If buffer is full, allocate more memory
  if (bytesRead == bufferSize)
  {
    (bufferSize > 0 ? bufferSize : ++bufferSize) += 1; // Double the buffer size
    buffer = (char *)realloc(buffer, bufferSize);
  }

  if (c == ESCAPE_BYTE)
  {
    char d = ser.read();
    buffer[bytesRead] = (d ^ ESCAPE_BYTE);
    bytesRead++;
  }
  else
  {
    buffer[bytesRead] = c;
    bytesRead++;
  }

  if (bytesRead >= MAX_MESSAGE_SIZE)
  {
    // Buffer overflow, reset buffer
    bytesRead = 0;
  }

  // return bytesRead;
}

// @deprecated
void SerialCom::serialEvent()
{
  if (!Serial)
  {
    return;
  }
  /*if (!isConnected())
  {
    return;
  }*/
  if (!isLoaded())
  {
    return;
  }

  uint8_t buffer[MAX_MESSAGE_SIZE];
  int len = decodeDataSerial(Serial, (char *)buffer);

  if (len > 0)
  {
    /*size_t heapUsed;
    size_t heapUsedAtStart;
    heapUsedAtStart = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    Serial.print("heap free: ");
    Serial.println(heapUsedAtStart);
    delay(20);*/

    char cmd = buffer[0];
    char op = buffer[1];

    switch (cmd)
    {
    case CMD_INIT:
    {
      handleInitCmd(op);
      break;
    }
    case AUTO_OFF_LENGTH:
      break;
    case AUTO_OFF_TOGGLE:
    {
      uint8_t op = buffer[1];
      uint8_t val;
      if (op == Operation::OP_GET)
      {
        bool enb = xAerPID1.AUTO_OFF_ENB;
        val = enb ? 0x01 : 0x00;
      }
      else
      {
        val = buffer[2];
        bool enb = val > 0 ? true : false;
        xAerPID1.AUTO_OFF_ENB = enb;
        aerManager->updateFlashStor(true);
        aerManager->setPressTick(250);
      }
      SerialCmdOp *reply = new SerialCmdOp(SerialCommand::AUTO_OFF_TOGGLE);
      reply->Val(val);
      reply->build();
      reply->emit();
      delete reply;
      break;
    }
    case ADJUST_AMOUNT:
      break;
    case CMD_BLE:
      break;
    case BUMP_AMOUNT:
    {
      double val;
      uint8_t op = buffer[1];
      if (op == Operation::OP_SET)
      {
        byteDouble bd;
        for (int i = 0; i < sizeof(double); i++)
        {
          bd.bytes[i] = buffer[2 + i];
        }
        if (aerManager->getReadingType() == ThermalUnitsType::FAHRENHEIT)
        { // convert to C
          Serial.printf(">> Bump-In(f): %f \n", bd.value);
          val = toCelsius(bd.value);
        }
        else if (aerManager->getReadingType() == ThermalUnitsType::KELVIN)
        { // convert to c
          Serial.printf(">> Bump-In(k): %f \n", bd.value);
          val = fromKelvin(bd.value);
        }
        else
        { // is c
          val = bd.value;
        }
        Serial.printf(">> Bump Amount(c): %f \n", val);
        // xAerPID1.BUMP_AMT = val;
        // aerManager->updateTempStor(true);
        // aerManager->setPressTick(250);
      }
      else
      {
        val = aerManager->getBump()->getTemp();
      }

      SerialCmdOp *reply = new SerialCmdOp(SerialCommand::CMD_TEMP);
      reply->Op(op);
      reply->Val(val);
      reply->build();
      reply->emit();
      delete reply;
    }
    break;
    case SerialCommand::BUMP_LENGTH:
    {
      int val;
      uint8_t op = buffer[1];
      if (op == Operation::OP_SET)
      {
        byteDouble bd;
        for (int i = 0; i < sizeof(double); i++)
        {
          bd.bytes[i] = buffer[2 + i];
        }
        val = int(bd.value);
        Serial.printf(">> Bump Length(s): %u \n", val);
        // xAerPID1.BUMP_TIME = val;
        // aerManager->updateTempStor(true);
        // aerManager->setPressTick(250);
      }
    }
    break;
    case SerialCommand::BUMP_TOGGLE:
    {
      uint8_t op = buffer[1];
      uint8_t val;
      if (op == Operation::OP_GET)
      {
        bool enb = aerManager->getBump()->getEnabled(0);
        val = enb ? 0x01 : 0x00;
      }
      else
      {
        val = buffer[2];
        bool enb = val > 0 ? true : false;
        aerManager->getBump()->setEnabled(0, enb);
      }
      SerialCmdOp *reply = new SerialCmdOp(SerialCommand::BUMP_TOGGLE);
      reply->Val(val);
      reply->build();
      reply->emit();
      delete reply;
    }
    break;
    case COIL_TOGGLE:
    {
      char param = buffer[2];
      handleCoilCmd(param, op);
      break;
    }
    case CMD_ESP:
      break;
    case FAV_1:
    case FAV_2:
    case FAV_3:
    case FAV_4:
      break;
    case CMD_LED:
    {
      char param = buffer[2];
      char value = buffer[3];
      handleLedCmd(param, op, value);
      break;
    }
    case SerialCommand::CMD_PID:
    {
      uint8_t op = buffer[1];
      uint8_t prm = buffer[2];
      switch (prm)
      {
      case PARAM_PID::PARAM_PID_P:
      {
        double val;
        if (op == OP_GET)
        {
          val = xAerPID1.kP;
        }
        else if (op == OP_SET)
        {
          byteDouble bd;
          for (int i = 0; i < sizeof(double); i++)
          {
            bd.bytes[i] = buffer[3 + i];
          }
          val = xAerPID1.kP = bd.value;
          Serial.println(">>> P Val >> " + String(val));
          xAerPID1.setTunings();
          aerManager->setPressTick(600);
          xAerPID1.pid_saved = false;
        }

        SerialCmdOp *reply = new SerialCmdOp(SerialCommand::CMD_PID);
        reply->Param(prm);
        reply->Val(val);
        reply->build();
        reply->emit();
        delete reply;
      }
      break;
      case PARAM_PID::PARAM_PID_I:
      {
        double val;
        if (op == OP_GET)
        {
          val = xAerPID1.kI;
        }
        else if (op == OP_SET)
        {
          byteDouble bd;
          for (int i = 0; i < sizeof(double); i++)
          {
            bd.bytes[i] = buffer[3 + i];
          }
          val = xAerPID1.kI = bd.value;
          Serial.println(">>> I Val >> " + String(val, 4));
          xAerPID1.setTunings();
          aerManager->setPressTick(600);
          xAerPID1.pid_saved = false;
        }
        SerialCmdOp *reply = new SerialCmdOp(SerialCommand::CMD_PID);
        reply->Param(prm);
        reply->Val(val);
        reply->build();
        reply->emit();
        delete reply;
      }
      break;
      case PARAM_PID::PARAM_PID_D:
      {
        double val;
        if (op == OP_GET)
        {
          val = xAerPID1.kD;
        }
        else if (op == OP_SET)
        {
          byteDouble bd;
          for (int i = 0; i < sizeof(double); i++)
          {
            bd.bytes[i] = buffer[3 + i];
          }
          val = xAerPID1.kD = bd.value;
          Serial.println(">>> D Val >> " + String(val));
          xAerPID1.setTunings();
          aerManager->setPressTick(600);
          xAerPID1.pid_saved = false;
        }
        SerialCmdOp *reply = new SerialCmdOp(SerialCommand::CMD_PID);
        reply->Param(prm);
        reply->Val(val);
        reply->build();
        reply->emit();
        delete reply;
      }
      break;
      }
    }
    break;
    case SerialCommand::CMD_TEMP:
    {
      double val;
      uint8_t op = buffer[1];
      if (op == Operation::OP_SET)
      {
        byteDouble bd;
        for (int i = 0; i < sizeof(double); i++)
        {
          bd.bytes[i] = buffer[2 + i];
        }
        if (aerManager->getReadingType() == ThermalUnitsType::FAHRENHEIT)
        { // convert to C
          Serial.printf(">> Temp-In(f): %f \n", bd.value);
          val = toCelsius(bd.value);
        }
        else if (aerManager->getReadingType() == ThermalUnitsType::KELVIN)
        { // convert to c
          Serial.printf(">> Temp-In(k): %f \n", bd.value);
          val = fromKelvin(bd.value);
        }
        else
        { // is c
          val = bd.value;
        }
        Serial.printf(">> Set Temp(c): %f \n", val);
        xAerPID1.SET_TEMP = val; // val = xAerPID1.SET_TEMP = bd.value;
        aerManager->updateTempStor(0, true);
        aerManager->setPressTick(250);
      }
      else
      {
        val = xAerPID1.SET_TEMP;
      }

      SerialCmdOp *reply = new SerialCmdOp(SerialCommand::CMD_TEMP);
      reply->Op(op);
      reply->Val(val);
      reply->build();
      reply->emit();
      delete reply;
    }
    break;
    case SerialCommand::CMD_WIFI:
    {
      uint8_t op = buffer[1];
      uint8_t prm = buffer[2];
      if (prm == WIFI::PARAM_JOIN_WIFI)
      {
        char val[64];
        for (int i = 0; i < 64; i++)
        {
          if (buffer[3 + i] == 0x00)
          {
            val[i] = '\0';
            break;
          }
          val[i] = buffer[3 + i];
        }
        String ssid_pw = String(val);
        SplitterClass splitter = SplitterClass();
        splitter.split(ssid_pw, ":");
        String ssid = splitter.contents()[0];
        String pass = splitter.contents()[1];
        splitter.dispose();
        commstor.setSSID((char *)ssid.c_str());
        commstor.setPSK((char *)pass.c_str());
        commstor.setSSIDSet(true);
        commstor.saveWiFi();

        SerialCmdOp *reply = new SerialCmdOp(SerialCommand::CMD_WIFI);
        reply->Param(prm);
        reply->Op(op);
        reply->Val((uint8_t)0x01);
        reply->build();
        reply->emit();
        delete reply;
      }
      else if (prm == WIFI::PARAM_RESET_WIFI)
      {
        commstor.setSSID("");
        commstor.setPSK("");
        commstor.setSSIDSet(false);
        commstor.saveWiFi();

        SerialCmdOp *reply = new SerialCmdOp(SerialCommand::CMD_WIFI);
        reply->Param(prm);
        reply->Op(op);
        reply->Val((uint8_t)0x01);
        reply->build();
        reply->emit();
        delete reply;
      }
    }
    break;
    default:
      break;
    }
    dispose();

    /*heapUsed = heapUsedAtStart - heap_caps_get_free_size(MALLOC_CAP_8BIT);
    Serial.print("heap used: ");
    Serial.println(heapUsed);*/
  }
}

void SerialCom::handleEventBuffer()
{
  if (!_hasMessage || messageStarted)
  {
    return;
  }

  char *buffer = getDecoded();

  char cmd = buffer[0];
  char op = buffer[1];

  switch (cmd)
  {
  case CMD_INIT:
  {
    handleInitCmd(op);
    break;
  }
  case AUTO_OFF_LENGTH:
    break;
  case AUTO_OFF_TOGGLE:
  {
    uint8_t op = buffer[1];
    uint8_t val;
    if (op == Operation::OP_GET)
    {
      bool enb = xAerPID1.AUTO_OFF_ENB;
      val = enb ? 0x01 : 0x00;
    }
    else
    {
      val = buffer[2];
      bool enb = val > 0 ? true : false;
      xAerPID1.AUTO_OFF_ENB = enb;
      aerManager->updateFlashStor(true);
      aerManager->setPressTick(250);
    }
    SerialCmdOp *reply = new SerialCmdOp(SerialCommand::AUTO_OFF_TOGGLE);
    reply->Val(val);
    reply->build();
    reply->emit();
    delete reply;
    break;
  }
  case ADJUST_AMOUNT:
    break;
  case CMD_BLE:
    break;

    {
      double val;
      uint8_t op = buffer[1];
      if (op == Operation::OP_SET)
      {
        byteDouble bd;
        for (int i = 0; i < sizeof(double); i++)
        {
          bd.bytes[i] = buffer[2 + i];
        }
        if (aerManager->getReadingType() == ThermalUnitsType::FAHRENHEIT)
        { // convert to C
          Serial.printf(">> Bump-In(f): %f \n", bd.value);
          val = toCelsius(bd.value);
        }
        else if (aerManager->getReadingType() == ThermalUnitsType::KELVIN)
        { // convert to c
          Serial.printf(">> Bump-In(k): %f \n", bd.value);
          val = fromKelvin(bd.value);
        }
        else
        { // is c
          val = bd.value;
        }
        Serial.printf(">> Bump Amount(c): %f \n", val);
        // xAerPID1.BUMP_AMT = val;
        // aerManager->updateTempStor(true);
        // aerManager->setPressTick(250);
      }
      else
      {
        val = aerManager->getBump()->getTemp();
      }

      SerialCmdOp *reply = new SerialCmdOp(SerialCommand::CMD_TEMP);
      reply->Op(op);
      reply->Val(val);
      reply->build();
      reply->emit();
      delete reply;
    }
    break;
  case SerialCommand::BUMP_LENGTH:
  {
    int val;
    uint8_t op = buffer[1];
    if (op == Operation::OP_SET)
    {
      byteDouble bd;
      for (int i = 0; i < sizeof(double); i++)
      {
        bd.bytes[i] = buffer[2 + i];
      }
      val = int(bd.value);
      Serial.printf(">> Bump Length(s): %u \n", val);
      // xAerPID1.BUMP_TIME = val;
      // aerManager->updateTempStor(true);
      // aerManager->setPressTick(250);
    }
  }
  break;
  case SerialCommand::BUMP_TOGGLE:
  {
    uint8_t op = buffer[1];
    uint8_t val;
    if (op == Operation::OP_GET)
    {
      bool enb = aerManager->getBump()->getEnabled(0);
      val = enb ? 0x01 : 0x00;
    }
    else
    {
      val = buffer[2];
      bool enb = val > 0 ? true : false;
      aerManager->getBump()->setEnabled(0, enb);
    }
    SerialCmdOp *reply = new SerialCmdOp(SerialCommand::BUMP_TOGGLE);
    reply->Val(val);
    reply->build();
    reply->emit();
    delete reply;
  }
  break;
  case COIL_TOGGLE:
  {
    char param = buffer[2];
    handleCoilCmd(param, op);
    break;
  }
  case CMD_ESP:
    break;
  case FAV_1:
  case FAV_2:
  case FAV_3:
  case FAV_4:
    break;
  case CMD_LED:
  {
    char param = buffer[2];
    char value = buffer[3];
    handleLedCmd(param, op, value);
    break;
  }
  case SerialCommand::CMD_PID:
  {
    uint8_t op = buffer[1];
    uint8_t prm = buffer[2];
    switch (prm)
    {
    case PARAM_PID::PARAM_PID_P:
    {
      double val;
      if (op == OP_GET)
      {
        val = xAerPID1.kP;
      }
      else if (op == OP_SET)
      {
        byteDouble bd;
        for (int i = 0; i < sizeof(double); i++)
        {
          bd.bytes[i] = buffer[3 + i];
        }
        val = xAerPID1.kP = bd.value;
        Serial.println(">>> P Val >> " + String(val));
        xAerPID1.setTunings();
        aerManager->setPressTick(600);
        xAerPID1.pid_saved = false;
      }

      SerialCmdOp *reply = new SerialCmdOp(SerialCommand::CMD_PID);
      reply->Param(prm);
      reply->Val(val);
      reply->build();
      reply->emit();
      delete reply;
    }
    break;
    case PARAM_PID::PARAM_PID_I:
    {
      double val;
      if (op == OP_GET)
      {
        val = xAerPID1.kI;
      }
      else if (op == OP_SET)
      {
        byteDouble bd;
        for (int i = 0; i < sizeof(double); i++)
        {
          bd.bytes[i] = buffer[3 + i];
        }
        val = xAerPID1.kI = bd.value;
        Serial.println(">>> I Val >> " + String(val, 4));
        xAerPID1.setTunings();
        aerManager->setPressTick(600);
        xAerPID1.pid_saved = false;
      }
      SerialCmdOp *reply = new SerialCmdOp(SerialCommand::CMD_PID);
      reply->Param(prm);
      reply->Val(val);
      reply->build();
      reply->emit();
      delete reply;
    }
    break;
    case PARAM_PID::PARAM_PID_D:
    {
      double val;
      if (op == OP_GET)
      {
        val = xAerPID1.kD;
      }
      else if (op == OP_SET)
      {
        byteDouble bd;
        for (int i = 0; i < sizeof(double); i++)
        {
          bd.bytes[i] = buffer[3 + i];
        }
        val = xAerPID1.kD = bd.value;
        Serial.println(">>> D Val >> " + String(val));
        xAerPID1.setTunings();
        aerManager->setPressTick(600);
        xAerPID1.pid_saved = false;
      }
      SerialCmdOp *reply = new SerialCmdOp(SerialCommand::CMD_PID);
      reply->Param(prm);
      reply->Val(val);
      reply->build();
      reply->emit();
      delete reply;
    }
    break;
    }
  }
  break;
  case SerialCommand::CMD_TEMP:
  {
    double val;
    uint8_t op = buffer[1];
    if (op == Operation::OP_SET)
    {
      byteDouble bd;
      for (int i = 0; i < sizeof(double); i++)
      {
        bd.bytes[i] = buffer[2 + i];
      }
      if (aerManager->getReadingType() == ThermalUnitsType::FAHRENHEIT)
      { // convert to C
        Serial.printf(">> Temp-In(f): %f \n", bd.value);
        val = toCelsius(bd.value);
      }
      else if (aerManager->getReadingType() == ThermalUnitsType::KELVIN)
      { // convert to c
        Serial.printf(">> Temp-In(k): %f \n", bd.value);
        val = fromKelvin(bd.value);
      }
      else
      { // is c
        val = bd.value;
      }
      Serial.printf(">> Set Temp(c): %f \n", val);
      xAerPID1.SET_TEMP = val; // val = xAerPID1.SET_TEMP = bd.value;
      aerManager->updateTempStor(0, true);
      aerManager->setPressTick(250);
    }
    else
    {
      val = xAerPID1.SET_TEMP;
    }

    SerialCmdOp *reply = new SerialCmdOp(SerialCommand::CMD_TEMP);
    reply->Op(op);
    reply->Val(val);
    reply->build();
    reply->emit();
    delete reply;
  }
  break;
  case SerialCommand::CMD_WIFI:
  {
    uint8_t op = buffer[1];
    uint8_t prm = buffer[2];
    if (prm == WIFI::PARAM_JOIN_WIFI)
    {
      char val[64];
      for (int i = 0; i < 64; i++)
      {
        if (buffer[3 + i] == 0x00)
        {
          val[i] = '\0';
          break;
        }
        val[i] = buffer[3 + i];
      }
      String ssid_pw = String(val);
      SplitterClass splitter = SplitterClass();
      splitter.split(ssid_pw, ":");
      String ssid = splitter.contents()[0];
      String pass = splitter.contents()[1];
      splitter.dispose();
      commstor.setSSID((char *)ssid.c_str());
      commstor.setPSK((char *)pass.c_str());
      commstor.setSSIDSet(true);
      commstor.saveWiFi();

      SerialCmdOp *reply = new SerialCmdOp(SerialCommand::CMD_WIFI);
      reply->Param(prm);
      reply->Op(op);
      reply->Val((uint8_t)0x01);
      reply->build();
      reply->emit();
      delete reply;
    }
    else if (prm == WIFI::PARAM_RESET_WIFI)
    {
      commstor.setSSID("");
      commstor.setPSK("");
      commstor.setSSIDSet(false);
      commstor.saveWiFi();

      SerialCmdOp *reply = new SerialCmdOp(SerialCommand::CMD_WIFI);
      reply->Param(prm);
      reply->Op(op);
      reply->Val((uint8_t)0x01);
      reply->build();
      reply->emit();
      delete reply;
    }
  }
  break;
  case SerialCommand::CMD_DEBUG:
  {
    uint8_t val;
    uint8_t op = buffer[1];
    if (op == Operation::OP_SET)
    {
      val = buffer[2];
      if (xSemaphoreTake(i2c1_mutex, 200) == pdTRUE)
      {
        Wire.beginTransmission(PID_MONITOR_ADDR);
        if (val == 0)
        {
          Wire.write(200);
          aerManager->setEnabledDTR(false);
        }
        else
        {
          Wire.write(100);
          aerManager->setEnabledDTR(true);
          val = 1;
        }
        Wire.endTransmission();
        xSemaphoreGive(i2c1_mutex);
      }
    }
    else if (op == Operation::OP_GET)
    {
      val = aerManager->isEnabledDTR() ? 1 : 0;
    }
    else
    {
      break;
    }
    SerialCmdOp *reply = new SerialCmdOp(SerialCommand::CMD_DEBUG);
    reply->Op(op);
    reply->Val(val);
    reply->build();
    reply->emit();
    delete reply;
    break;
  }
  default:
    break;
  }
  dispose();
}

void SerialCom::handleEventBuffer(char *buffer)
{
  char cmd = buffer[0];
  char op = buffer[1];

  switch (cmd)
  {
  case CMD_INIT:
  {
    handleInitCmd(op);
    break;
  }
  case AUTO_OFF_LENGTH:
    break;
  case AUTO_OFF_TOGGLE:
    break;
  case ADJUST_AMOUNT:
    break;
  case CMD_BLE:
    break;
  case BUMP_AMOUNT:
    break;
  case BUMP_LENGTH:
    break;
  case BUMP_TOGGLE:
    break;
  case COIL_TOGGLE:
  {
    char param = buffer[2];
    handleCoilCmd(param, op);
    break;
  }
  case CMD_ESP:
    break;
  case FAV_1:
  case FAV_2:
  case FAV_3:
  case FAV_4:
    break;
  case CMD_LED:
  {

    break;
  }
  case CMD_PID:
    break;
  case CMD_TEMP:
    break;
  case CMD_WIFI:
    break;
  default:
    break;
  }
  dispose();
}

void SerialCom::handleInitCmd(char op)
{
  if (op == Operation::OP_GET)
  {
    const xVersion v = aerManager->getVersion()->getVer();
    const xVersion vw = aerManager->getVersionWeb()->getVer();
    MessagePack messagePack = MessagePack(v.major, v.minor, v.build, vw.major, vw.minor, vw.build);
    enPackFill(&messagePack);

    uint len = messagePack.fetchInitMessage();
    if (len > 0 && messagePack.isPackComplete())
    {
      SerialCmdOp *sco = new SerialCmdOp((char *)messagePack.getPack(), len);
      sco->build();
      sco->emit();
      delete sco;
    }
    messagePack.dispose();
    aerManager->setActiveUsb(true);
  }
}

void SerialCom::handleCoilCmd(char param, char op)
{
  if (op == Operation::OP_GET)
  {
    bool en = aerManager->getAerPID(0)->PID_ON;
    SerialCmdOp *sco = new SerialCmdOp(COIL_TOGGLE);
    sco->Val(en);
    sco->build();
    sco->emit();
    delete sco;
  }
  else if (op == Operation::OP_SET)
  {
    bool enb = aerManager->getAerPID(0)->PID_ON = param > 0;
    SerialCmdOp *sco = (new SerialCmdOp(COIL_TOGGLE));
    sco->Val(enb);
    sco->build();
    sco->emit();
    // SerialCmdOp sco = (new SerialCmdOp(COIL_TOGGLE))->Val(enb);
    // sco.build();
    // sco.emit();
    delete sco;
  }
}

void SerialCom::handleLedCmd(char param, char op, char dat)
{
}

void SerialCom::updateClients()
{
  SerialCmdOp *cmd = new SerialCmdOp(SerialCommand::CMD_STATUS);
  cmd->Op(Operation::OP_GET);
  cmd->Val(xAerPID1.PID_ON);
  cmd->Val(static_cast<uint16_t>(10.0 * xAerPID1.MES_TEMP));
  cmd->Val(static_cast<uint16_t>(10.0 * xAerPID1.AVG_TEMP));
  cmd->Val(static_cast<uint16_t>(10.0 * xAerPID1.SET_TEMP));
  cmd->build();
  cmd->emit();
  delete cmd;
}