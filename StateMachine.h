#ifndef __WHSM_H_
#define __WHSM_H_

#include "Types.h"

//#define SMDEBUG

#ifdef SMDEBUG
#define SMDEBUG_PRINT(x) Serial.print(x)
#define SMDEBUG_PRINTLN(x) Serial.println(x)
#else
#define SMDEBUG_PRINT(x) 
#define SMDEBUG_PRINTLN(X)
#endif

#define DEFAULT_SPL   47.7777777
#define DEFAULT_SPH   48.8888888
#define DEFAULT_PFL   1.0

extern WaterHeaterState whstate;
extern WaterHeaterControl whcontrol;

bool initWaterHeaterSM();
bool doWaterHeaterSM();
bool doCalculateWaterFlow();
bool doWaterHeaterPauseCheck();

#endif
