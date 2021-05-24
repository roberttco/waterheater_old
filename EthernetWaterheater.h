#ifndef __EWH_H_
#define __EWH_H_

//#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#else
#define DEBUG_PRINT(x) 
#define DEBUG_PRINTLN(X)
#endif

#define SD_BUFLEN 32

// max temp limits
#define MAX_TEMPC 50 // 122F

// substate intervals
#define WHSM_INVOKE_INTERVAL 500
#define FLOW_CALC_INTERVAL 1000
#define TEMP_UPDATE_INTERVAL 5000
#define RADIO_SEND_INTERVAL 1000
#define NETWORK_RECONNECT_INTERVAL 2000

#endif
