#ifndef __MYEEPROM_H_
#define __MYEEPROM_H_

#include <EEPROM.h>

//#define EPDEBUG

#ifdef EPDEBUG
#define EPDEBUG_PRINT(x) Serial.print(x)
#define EPDEBUG_PRINTLN(x) Serial.println(x)
#else
#define EPDEBUG_PRINT(x) 
#define EPDEBUG_PRINTLN(X)
#endif

// EEPROM variables and defines
#define SPL_ADDRESS 0
#define SPH_ADDRESS 4
#define PFL_ADDRESS 8
#define FUDL_ADDRESS 12
#define FPDL_ADDRESS 16
#define IP_ADDRESS 20
#define RADIO_ID_ADDRESS 24
#define RADIO_CHANNEL_ADDRESS 25
#define LAST_ADDRESS 26

void setDefaultEEPROMSettings(uint16_t lastAddress);
bool getEEPROMFloatValue(int address,float *variable, float default_value, bool save);
bool getEEPROMUnsignedLongValue(int address,unsigned long *variable, unsigned long default_value, bool save);
bool getEEPROMByteValue(int address,unsigned char *variable, unsigned char default_value, bool save);
bool getEEPROMIntValue(int address,int *variable, int default_value, bool save);

#endif
