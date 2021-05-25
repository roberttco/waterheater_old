#include <Arduino.h>
#include <arduino-timer.h>

#include "Types.h"
#include "StateMachine.h"
#include "TemperatureSensor.h"
#include "EthernetWaterheater.h"
#include "MyEEPROM.h"
#include "Pins.h"

WaterHeaterState whstate;
WaterHeaterControl whcontrol;

volatile uint16_t flowSensorPulseCount = 0;
uint16_t flow_count;

auto pause_timer = timer_create_default(); // create a timer with default settings
Timer<>::Task pause_task;
Timer<>::Task unpause_task;

bool smUnpause();
bool smPause();

void flowSensorISR(void)
{
  flowSensorPulseCount += 1;
}

bool initWaterHeaterSM()
{
  // get values from EEPROM.  If they are out of bounds then set them to the defaults and save 
  // back to EEPROM for next time.

  getEEPROMFloatValue(SPH_ADDRESS,  &whcontrol.sph_c,           48.8888889  ,true);
  getEEPROMFloatValue(SPL_ADDRESS,  &whcontrol.spl_c,           47.7777778  ,true);
  getEEPROMFloatValue(PFL_ADDRESS,  &whcontrol.pfl_lpm,         1.0         ,true);
  getEEPROMIntValue(FUDL_ADDRESS,   &whcontrol.unpausedelayms,  3000        ,true);
  getEEPROMIntValue(FPDL_ADDRESS,   &whcontrol.pausedelayms,    3100        ,true);

#ifdef SMDEBUG
  Serial.print (F("Startup SPH = "));
  Serial.println (whcontrol.sph_c);
  Serial.print (F("Startup SPL = "));
  Serial.println (whcontrol.spl_c);
  Serial.print (F("Startup PFL = "));
  Serial.println (whcontrol.pfl_lpm);
  Serial.print (F("Startup FUDL = "));
  Serial.println (whcontrol.unpausedelayms);
  Serial.print (F("Startup FPDL = "));
  Serial.println (whcontrol.pausedelayms);
#endif

  // start paused to avoid "pulsing" the valve and pump at init time
  whstate.paused = true;
  whstate.heating = true;

  // make sure valve and pump are off at boot time
  digitalWrite(N_RELAY1_PIN,HIGH);    // relay is active low
  digitalWrite(N_RELAY2_PIN,HIGH);    // relay is active low

  pinMode(FLOW_SENSE_PIN,INPUT);
  attachInterrupt(0,flowSensorISR,RISING);

  return true;
}

void openValveAndStartPump()
{
  if (whstate.valve == false || whstate.pump == false)
  {
    digitalWrite(N_RELAY1_PIN,LOW);    // relay is active low
    whstate.valve = true;
    
    delay(1000);
    
    digitalWrite(N_RELAY2_PIN,LOW);    // relay is active low
    
    whstate.pump = true;
    whstate.heating = true;
  }
}

void closeValveAndTurnOffPump()
{
  if (whstate.valve == true || whstate.pump == true)
  {
    digitalWrite(N_RELAY2_PIN,HIGH);    // relay is active low
    whstate.valve = false;
    
    delay(1000);
    
    digitalWrite(N_RELAY1_PIN,HIGH);    // relay is active low
    
    whstate.pump = false;
    whstate.heating = false;
  }
}

static unsigned long lastFlowMeasurementTime = 0;

bool doCalculateWaterFlow(void *arg)
{
  unsigned long timeNow = millis();
  
  noInterrupts();
  flow_count = flowSensorPulseCount;
  flowSensorPulseCount = 0;
  interrupts();

  if (timeNow > lastFlowMeasurementTime)  // millis counter rolled over so dont bother calculating
  {
    unsigned long measurePeriod = timeNow - lastFlowMeasurementTime;
    
    float measureFrequency = (float)flow_count / (float)measurePeriod;
    whstate.flowrate_lpm = measureFrequency * 5.5;  // F * 5.5 = liters/min
  }
  
  lastFlowMeasurementTime = timeNow;
  return true;
}

bool doWaterHeaterPauseCheck(void *arg)
{
  pause_timer.tick();
  return true;
}

bool smUnpause(void *arg)
{
  // if the flow is over the limit of if the delay is set to 0, then cancel the timer
  if (whstate.flowrate_lpm >= whcontrol.pfl_lpm)
  {
    SMDEBUG_PRINTLN(F("!un_pse"));
    unpause_task = 0;
  }
  else
  {
    SMDEBUG_PRINTLN(F("un_pse"));
    whstate.paused = false;
    pause_task = 0;
  }

  return false;
}

bool smPause(void *arg)
{
  if (whstate.flowrate_lpm < whcontrol.pfl_lpm)
  {
    SMDEBUG_PRINTLN(F("!pse"));
    pause_task = 0;
  }
  else
  {
    SMDEBUG_PRINTLN(F("pse"));
    unpause_task = 0;
    
    whstate.heating = false;
    whstate.paused  = true;
    closeValveAndTurnOffPump();
  }
  return false;
}

bool doWaterHeaterSM(void *arg)
{
  // temp is auto updated every TEMP_UPDATE_INTERVAL milliseconds
  whstate.temperature_c = getTemperature(false);

  // if disable shut it all down
  if (whcontrol.enable == false)
  {
    whstate.forced  = false;
    whstate.heating = false;
    whstate.paused  = false;

    pause_timer.cancel();

    closeValveAndTurnOffPump();
  }
  else if (whstate.paused == true)
  {
    SMDEBUG_PRINT(F("Paused - fr_lpm = "));
    SMDEBUG_PRINT(whstate.flowrate_lpm);
    SMDEBUG_PRINT(F(", pfl_lpm = "));
    SMDEBUG_PRINT(whcontrol.pfl_lpm);
    SMDEBUG_PRINT(F(", unpause_task = "));
    SMDEBUG_PRINTLN(unpause_task == 0 ? "NULL" : "NOT_NULL");
    
    if ((whstate.flowrate_lpm < whcontrol.pfl_lpm) && (unpause_task == 0))
    {
      unpause_task = pause_timer.in(whcontrol.unpausedelayms,smUnpause);        
    }
  }
  else
  {
    SMDEBUG_PRINTLN("Not paused");
    if ((whstate.temperature_c < whcontrol.sph_c) && (whstate.heating == true))
    {
      openValveAndStartPump();
    }

    if ((whstate.temperature_c < whcontrol.sph_c) && (whcontrol.force == true))
    {
      SMDEBUG_PRINTLN("Forced heating");
      whstate.forced = true;
      openValveAndStartPump();
    }

    if ((whstate.flowrate_lpm >= whcontrol.pfl_lpm) && (pause_task == 0))
    {
      SMDEBUG_PRINTLN("Gonna pause");
      pause_task = pause_timer.in(whcontrol.pausedelayms,smPause);
    }

    if (whstate.temperature_c < whcontrol.spl_c)
    {
      SMDEBUG_PRINTLN("Heating");
      openValveAndStartPump();
    }

    if (whstate.temperature_c >= whcontrol.sph_c)
    {
      SMDEBUG_PRINTLN("Done heating");
      whstate.forced = false;
      whstate.heating = false;
      
      // clear the control flag so it is not perpetually forced 
      whcontrol.force = false;

      
      closeValveAndTurnOffPump();
    }
  }

  return true;
}
