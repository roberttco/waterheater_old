#ifndef __TEMPSENSOR_H_
#define __TEMPSENSOR_H_


//#define TSDEBUG

#ifdef TSDEBUG
#define TSDEBUG_PRINT(x) Serial.print(x)
#define TSDEBUG_PRINTLN(x) Serial.println(x)
#else
#define TSDEBUG_PRINT(x) 
#define TSDEBUG_PRINTLN(X)
#endif

float getTemperature(bool reRead);
bool initTemperatureSensor();
bool updateTemperature();

#endif
