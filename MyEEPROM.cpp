#include <Arduino.h>
#include "MyEEPROM.h"

#include "Radio.h"
#include "StateMachine.h"

void setDefaultEEPROMSettings(uint16_t lastAddress)
{
  EPDEBUG_PRINTLN(F("Setting default EEPROM values"));

  // set the default values in the EEPROM
  EEPROM.put(SPL_ADDRESS,(float)DEFAULT_SPL);
  EEPROM.put(SPH_ADDRESS,(float)DEFAULT_SPH);
  EEPROM.put(PFL_ADDRESS,(float)DEFAULT_PFL);
  EEPROM.put(FUDL_ADDRESS,(unsigned int)3000);
  EEPROM.put(FPDL_ADDRESS,(unsigned int)3100);
  EEPROM.put(IP_ADDRESS+0,(byte)0);
  EEPROM.put(IP_ADDRESS+1,(byte)0);
  EEPROM.put(IP_ADDRESS+2,(byte)0);
  EEPROM.put(IP_ADDRESS+3,(byte)0);
  EEPROM.put(RADIO_ID_ADDRESS,(byte)RADIO_ID);
  EEPROM.put(RADIO_CHANNEL_ADDRESS,(byte)RADIO_CHANNEL);
}

bool getEEPROMFloatValue(unsigned int address,float *variable, float default_value, bool save)
{
  EPDEBUG_PRINTLN(F("Getting EEPROM float value"));

  if (address > EEPROM.length())
  {
    return false;
  }

  float rval = 0.0;
  bool hadToSave = false;
  
  EEPROM.get(address,rval);
  
  if (isnan(rval) || rval == 0)
  {
    rval = default_value;
    if (save == true)
    {
      EPDEBUG_PRINTLN(F("Setting default"));
      EEPROM.put(address,rval);
      delay(25);  // wait for EEPROM write to complete
      hadToSave = true;
    }
  }

  EPDEBUG_PRINT(F("Got value "));
  EPDEBUG_PRINTLN(rval);

  *variable = rval;

  return hadToSave;
}

bool getEEPROMUnsignedLongValue(unsigned int address,unsigned long *variable, unsigned long default_value, bool save)
{
  EPDEBUG_PRINTLN(F("Getting EEPROM unsigned long value"));

  if (address > EEPROM.length())
  {
    return false;
  }

  unsigned long rval = 0L;
  bool hadToSave = false;
  
  EEPROM.get(address,rval);
  
  if (isnan(rval))
  {
    rval = default_value;
    if (save == true)
    {
      EPDEBUG_PRINTLN(F("Setting default"));
      EEPROM.put(address,rval);
      delay(25);  // wait for EEPROM write to complete
      hadToSave = true;
    }
  }

  EPDEBUG_PRINT(F("Got value "));
  EPDEBUG_PRINTLN(rval);

  *variable = rval;

  return hadToSave;
}

bool getEEPROMByteValue(unsigned int address,unsigned char *variable, unsigned char default_value, bool save)
{
  EPDEBUG_PRINTLN(F("Getting EEPROM unsigned char value"));

  if (address > EEPROM.length())
  {
    return false;
  }

  unsigned char rval = 0;
  bool hadToSave = false;
  
  EEPROM.get(address,rval);
  
  if (isnan(rval))
  {
    rval = default_value;
    if (save == true)
    {
      EPDEBUG_PRINTLN(F("Setting default"));
      EEPROM.put(address,rval);
      delay(25);  // wait for EEPROM write to complete
      hadToSave = true;
    }
  }

  EPDEBUG_PRINT(F("Got value "));
  EPDEBUG_PRINTLN(rval);

  *variable = rval;

  return hadToSave;
}

bool getEEPROMIntValue(unsigned int address,int *variable, int default_value, bool save)
{
  EPDEBUG_PRINTLN(F("Getting EEPROM int value"));

  if (address > EEPROM.length())
  {
    return false;
  }

  int rval = 0;
  bool hadToSave = false;
  
  EEPROM.get(address,rval);
  
  if (isnan(rval))
  {
    rval = default_value;
    if (save == true)
    {
      EPDEBUG_PRINTLN(F("Setting default"));
      EEPROM.put(address,rval);
      delay(25);  // wait for EEPROM write to complete
      hadToSave = true;
    }
  }

  EPDEBUG_PRINT(F("Got value "));
  EPDEBUG_PRINTLN(rval);

  *variable = rval;

  return hadToSave;
}
