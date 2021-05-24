#include <Arduino.h>
#include <NRFLite.h>

#include "Radio.h"
#include "MyEEPROM.h"
#include "Pins.h"

NRFLite _radio;
bool _radioOk = false;

uint8_t radioChannel = RADIO_CHANNEL;
uint8_t radioId = RADIO_ID;

void saveRadioConfig()
{
  EEPROM.put(RADIO_ID_ADDRESS,radioId);
  EEPROM.put(RADIO_CHANNEL_ADDRESS,radioChannel);
}

void setRadioId(uint8_t id)
{
  radioId = id;
}

void setRadioChannel(uint8_t channel)
{
  radioChannel = channel;
}

uint8_t getRadioId()
{
  return radioId;
}

uint8_t getRadioChannel()
{
  return radioChannel;
}

bool initRadio()
{
    getEEPROMByteValue(RADIO_ID_ADDRESS,&radioId,RADIO_ID,true);
    getEEPROMByteValue(RADIO_CHANNEL_ADDRESS,&radioChannel,RADIO_ID,true);
    
    if (!_radio.init(radioId, RADIO_CS_PIN, RADIO_CSN_PIN, NRFLite::BITRATE2MBPS, radioChannel))
    {
        RDEBUG_PRINTLN(F("Cannot communicate with radio"));
        _radioOk = false;
    }
    else
    {
      _radioOk = true;
    }
    
    RDEBUG_PRINT (F("Startup radio ID = "));
    RDEBUG_PRINTLN (radioId);
    RDEBUG_PRINT (F("Startup radio channel = "));
    RDEBUG_PRINTLN (radioChannel);

    return _radioOk;
}

bool radioOk()
{
  return _radioOk;
}

bool radioSend(uint8_t destination, void *arg, uint8_t arg_length)
{
  return (_radio.send(DESTINATION_RADIO_ID, arg, arg_length)); // Note how '&' must be placed in front of the variable name.
}
