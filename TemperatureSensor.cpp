#include "TemperatureSensor.h"
#include <DS18B20.h>
#include "Pins.h"

unsigned char dsAddress[8];
DS18B20 ds(DS_PIN);

float _lastTemperature = 0.0;
bool _initialized = false;

bool initTemperatureSensor()
{
  if (_initialized == true)
  {
    return true;
  }
  else
  {
    TSDEBUG_PRINT(F("Initializing temperature sensor."));
    
    // init DS18B20
    if (ds.selectNext())
    {
      ds.getAddress(dsAddress);
      ds.setResolution(12);
      _lastTemperature = ds.getTempC();
    }
    else
    {
      TSDEBUG_PRINTLN(F("No DS18B20 found.  Cannot continue"));
      return false;
    }

#ifdef TSDEBUG
    Serial.print("DS18B20 Address = ");
    for (uint8_t i = 0; i < 8; i++) {
       Serial.print(" ");
       Serial.print(dsAddress[i]);
    }
    Serial.println();
#endif
    
    _initialized = true;
    
    return true;
  }
}

float readTemperature()
{
  TSDEBUG_PRINT(F("Reading temperature from sensor ... "));
  
  if (_initialized == true)
  {
    _lastTemperature = ds.getTempC();
  }
  else
  {
    _lastTemperature = -1000.0;
  }

  TSDEBUG_PRINTLN(_lastTemperature);

  return _lastTemperature;
}

float getTemperature(bool reRead)
{
  if (reRead == true)
  {
    _lastTemperature = readTemperature();
  }
  
  return _lastTemperature;
}

bool updateTemperature(void *arg)
{
  readTemperature();
  return true;
}
