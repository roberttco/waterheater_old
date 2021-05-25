#include <Arduino.h>
#include <avr/wdt.h>
#include <arduino-timer.h>

//#include <SPI.h>
//#include <PetitFS.h>

#include "Pins.h"
#include "MyEEPROM.h"
#include "EthernetWaterheater.h"
#include "NetworkManagement.h"
#include "WebServer.h"
#include "Radio.h"
#include "TemperatureSensor.h"
#include "StateMachine.h"

// SD stuff
#ifdef PetitFS_h
FATFS fs;
boolean sdok = false;
#endif

auto timer = timer_create_default(); // create a timer with default settings

void flashError(byte errorcode)
{
  DEBUG_PRINT (F("Fatal Error "));
  DEBUG_PRINTLN(errorcode);
  
  while (1)
  {
    if (errorcode > 0)
    {
      for (byte i = 0; i < errorcode ; i++)
      {
        digitalWrite(ERRORLED_PIN,HIGH);
        delay(250);
        digitalWrite(ERRORLED_PIN,LOW);
        delay(250);
      }
      digitalWrite(ERRORLED_PIN,LOW);
      delay(2000);
    }
  }
}

void setup()
{
  Serial.begin(115200);       // for debugging

  pinMode(RESET_PIN,INPUT_PULLUP);
  pinMode(ERRORLED_PIN, OUTPUT);
  pinMode(N_RELAY1_PIN, OUTPUT);
  pinMode(N_RELAY2_PIN, OUTPUT);

  // turn off relays
  digitalWrite(N_RELAY1_PIN,HIGH);    // relay is active low
  digitalWrite(N_RELAY2_PIN,HIGH);    // relay is active low

  Serial.println (F("BOOT"));

  // if the reset pin is low then reset everything to defaults
  if (digitalRead(RESET_PIN) == 0)  
  {
    Serial.println ("Reset pin low - resetting EEPROM contents and controller to default values");
    setDefaultEEPROMSettings(LAST_ADDRESS);
  }

#ifdef PetitFS_h
  // Initialize SD and file system.
  if (pf_mount(&fs)) 
  {
    Serial.println(F("SD error"));
    flashError(2);
  }
  else
  {
    Serial.println(F("SD mounted"));
    sdok = true;
  }
#endif    

  if (!initRadio())
  {
    Serial.println(F("Radio init error"));
  }
  else
  {
    whstate.radio.id = getRadioId();
    whstate.radio.channel = getRadioChannel();
    
    Serial.print (F("Radio ID: "));
    Serial.print (whstate.radio.id);
    Serial.print (F(", Radio Channel: "));
    Serial.println (whstate.radio.channel);
  }

  if (!initTemperatureSensor())
  {
    Serial.println(F("No temperature sensor."));
    flashError(6);
  }

  if (!initWaterHeaterSM())
  {
    Serial.println(F("WHSM init error"));
    flashError(3);
  }

  if (!initNetwork())
  {
    Serial.println(F("Could not init network."));
  }
  
  if (!connectToNetwork(false))
  {
    Serial.println(F("Couldn't connect to network.  Hoping for radio control."));
  }
  else
  {
    Serial.println(getIpAddress());
    getIpAddress(&whstate.ip.ip[0], &whstate.ip.ip[1], &whstate.ip.ip[2], &whstate.ip.ip[3], &whstate.ip.dhcp);
  }

  whcontrol.enable = true;

  // tick the pause timer every 100ms
  timer.every(100,doWaterHeaterPauseCheck);

  timer.every(TEMP_UPDATE_INTERVAL,updateTemperature);
  timer.every(WHSM_INVOKE_INTERVAL,doWaterHeaterSM);
  timer.every(FLOW_CALC_INTERVAL,doCalculateWaterFlow);
  
  wdt_enable(WDTO_4S);
}

unsigned long lastSend = 0;
unsigned long lastNetCheck = 0;
void loop()
{
  timer.tick();

  // Can't do this with timer.every without some rework so do this here.  Check for a 
  // new connection (cable plug in) only oif the system is not heating.  This is because
  // a DHCP timeout is very long and may cause heating to go too far.
  if ((whstate.heating == false) && (millis() - lastNetCheck > NETWORK_RECONNECT_INTERVAL))
  {
    if (checkNetworkCablePlugin())
    {
      getIpAddress(&whstate.ip.ip[0], &whstate.ip.ip[1], &whstate.ip.ip[2], &whstate.ip.ip[3], &whstate.ip.dhcp);
    }
    lastNetCheck = millis();
  }

  if (networkConnected())
  {
    doWebServer();
    renewDhcpLease();
  }

  // Need to pass the information to a radio send function to use the timer.every method
  if (radioOk() && (millis() - lastSend > RADIO_SEND_INTERVAL))
  {
      DEBUG_PRINT(F("Sending state.  Length = "));
      DEBUG_PRINTLN(sizeof(whstate));
      radioSend(0, &whstate, sizeof(whstate));
      lastSend = millis(); 
  }

  wdt_reset();
}
