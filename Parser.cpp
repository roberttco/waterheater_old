#include "Parser.h"

int parseCommandLine(char *buf, char *path, int path_buf_len, float *floatval, int *intval, byte *byteval)
{
  PDEBUG_PRINTLN(F("*********** parsing new command"));
  
  int command = CMD_NONE;
  float f_rval = 0.0;
  int i_rval = 0;
  byte b_rval = 0;
  byte cmdlength = 0;

  // debugging
#ifdef PDEBUG
  {
    Serial.println(F("*** Parsing:"));
    byte dbug_i = 0;
    while (buf[dbug_i] != '\r' && buf[dbug_i] != '\n' && dbug_i < MAX_BUFLEN) { Serial.write(buf[dbug_i++]); }
    Serial.println("");
  }
#endif

  if (memcmp(buf,"GET",3) != 0)
  {
    return CMD_INVALID;
  }
  else
  {
    cmdlength = 3;
  }
    
  //Theoretically, the buffer contains the entire command line that is of interest (e.g. command and command path)

  byte beginningofpath = 0;
  byte pathlength = 0;
  bool foundHttp = false;
  for (byte pathfinder = MAX_BUFLEN-1; pathfinder > 0;pathfinder--)
  {
    // find the HTTP part of the command - i.e. GET /favicon.ico HTTP/1.1
    if (buf[pathfinder-3] == 'H' && buf[pathfinder-2] == 'T' && buf[pathfinder-1] == 'T' && buf[pathfinder] == 'P')
    {
      foundHttp = true;
      
      // buf[beginningofpath] is the first character in the path - for a GET command this is 5
      beginningofpath = cmdlength + 2;  

      // end of path - beginning of path (1-based this is a length and not a buffer offset).  5 for length of HTTP + leading space
      pathlength = (pathfinder - 5) - 4;  


      for (byte j = 0; j < pathlength && j < path_buf_len-1; path[j] = toupper(buf[j+beginningofpath]),j++);
      path[min(pathlength,path_buf_len)] = 0;
      break;
    }
  }
 
  if (foundHttp == false)
  {
    PDEBUG_PRINT(F("*** ERROR: couldn't find HTTP in buffer.  Try making MAX_BUFLEN larger"));
    return CMD_INVALID;
  }

  PDEBUG_PRINT(F("*** Command Path = '"));
  PDEBUG_PRINT(path);
  PDEBUG_PRINTLN("'"); 
  PDEBUG_PRINT(F("*** Path length = "));
  PDEBUG_PRINTLN(pathlength);
  
  // if pathlength == 0 then this is the root path (e.g. just the server URL with nothing after)
  switch (path[0])
  {
    case 'E':                           // only E command is Enable
      command = CMD_ENABLE;
      i_rval = atoi(&path[7]);
      break;
    case 'F':
      switch (path[1])
      {
        case 'A':                       // FAVICON
          command = CMD_FAVICON;
          break;
        case 'O':                       // FORCE
          command = CMD_FORCE;
          i_rval = atoi(&path[6]);
          break;
        case 'P':                       // FPDL
          command = CMD_SETFLOW_PAUSE_DELAY;
          i_rval = atoi(&path[5]);
          break;
        case 'U':                       // FUDL
          command = CMD_SETFLOW_UNPAUSE_DELAY;
          i_rval = atoi(&path[5]);
          break;
      }
      break;
    case 'S':
      switch (path[1])
      {
        case 'P':
        {
          switch (path[2])
          {
            case 'H': // SPH - hight set point
              command = CMD_SETSPH;
              f_rval = atof(&path[4]);
              break;
            case 'L': // SPL - low set point
              command = CMD_SETSPL;
              f_rval = atof(&path[4]);
              break;
          }
          break;
        case 'A': // SAVE - save settingds to EEPROM
          command = CMD_SAVE;
          break;
        }
      }
      break;
    case 'P':
      switch (path[1])
      {
        case 'F':
          switch (path[2])
          {
            case 'L':   // PFL - flow rate pause limit
              command = CMD_SETPFL;
              f_rval = atof(&path[4]);
              break;
          }
          break;
      }
      break;
    case 'R':
      switch (path[1])
      {
        case 'E':   // reboot
          // special case - make sure it really says reboot
          if (memcmp(path,"REBOOT",6) == 0)
          {
            command = CMD_REBOOT;
          }
          break;
        case 'I':   // RID - radio ID
          command = CMD_SET_RADIO_ID;
          i_rval = atoi(&path[4]);
          break;
        case 'C':   // RCH - radio channel
          command = CMD_SET_RADIO_CHANNEL;
          i_rval = atoi(&path[4]);
          break;
      }
      break;
    case 'I':
      switch (path[1])
      {
        case 'P': //IPx - set IP address
          switch (path[2])
          {
            case '1':
            case '2':
            case '3':
            case '4':
              b_rval = atoi(&path[4]);
              i_rval = path[2] - '0';
              command = CMD_SET_IP;
              break;
          }
          break;
      }
      break;
    default:
      command = CMD_JSON;
      break;
  }

  PDEBUG_PRINT(F("*** Command = "));
  PDEBUG_PRINTLN(command);
  PDEBUG_PRINT(F("*** Float val = "));
  PDEBUG_PRINTLN(f_rval);
  PDEBUG_PRINT(F("*** Int val = "));
  PDEBUG_PRINTLN(i_rval);

  *floatval = f_rval;
  *intval   = i_rval;
  *byteval  = b_rval;
  
  return command;
}
