#ifndef __EWHTYPES_H_
#define __EWHTYPES_H_

typedef struct _WaterHeaterState
{
  bool heating;
  bool pump;
  bool valve;
  bool forced;
  bool paused;
  float temperature_c;
  float flowrate_lpm;
  bool settingsSaved;
  byte ip[4];
  byte radio[2];
} WaterHeaterState;

typedef struct _WaterHeaterControl
{
  float sph_c;
  float spl_c;
  float pfl_lpm;
  int unpausedelayms;
  int pausedelayms;
  bool enable;
  bool force;
  byte ip[4];
  byte radio[2];
} WaterHeaterControl;

#endif
