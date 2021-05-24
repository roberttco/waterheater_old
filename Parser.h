#ifndef __PARSER_H__
#define __PARSER_H__

#include <Arduino.h>

//#define PDEBUG

#ifdef PDEBUG
#define PDEBUG_PRINT(x) Serial.print(x)
#define PDEBUG_PRINTLN(x) Serial.println(x)
#else
#define PDEBUG_PRINT(x) 
#define PDEBUG_PRINTLN(X)
#endif

// command parser
#define CMD_NONE 0
#define CMD_GET  1
#define CMD_JSON 2
#define CMD_FAVICON 3
#define CMD_STATUS 4
#define CMD_FORCE 5
#define CMD_ENABLE 6
#define CMD_SETSPL 7
#define CMD_SETSPH 8
#define CMD_SETPFL 9
#define CMD_SAVE 10
#define CMD_SETFLOW_PAUSE_DELAY 11
#define CMD_SETFLOW_UNPAUSE_DELAY 12
#define CMD_SET_IP 13
#define CMD_SET_RADIO_ID 14
#define CMD_SET_RADIO_CHANNEL 15
#define CMD_REBOOT 98
#define CMD_INVALID 255


// need to be bigger if ever parsing something longer than a single 8.3 file name (e.g. like form parameters)
#define MAX_BUFLEN 26
#define PATH_BUFLEN 15

// command parser
extern byte command;

int parseCommandLine(char *buf, char *path, int path_buf_len, float *floatval, int *intval, byte *byteval);

#endif
