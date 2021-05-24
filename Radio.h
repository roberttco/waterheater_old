#ifndef __RADIO_H_
#define __RADIO_H_

//#define RDEBUG

#ifdef RDEBUG
#define RDEBUG_PRINT(x) Serial.print(x)
#define RDEBUG_PRINTLN(x) Serial.println(x)
#else
#define RDEBUG_PRINT(x) 
#define RDEBUG_PRINTLN(X)
#endif

#define RADIO_ID              1   // Our radio's id.
#define DESTINATION_RADIO_ID  0   // Id of the radio we will transmit to.
#define RADIO_CHANNEL         100

bool initRadio();
void saveRadioConfig();
void setRadioId(uint8_t id);
void setRadioChannel(uint8_t id);
uint8_t getRadioId();
uint8_t getRadioChannel();
bool radioOk();
bool radioSend(uint8_t destination, void *arg, uint8_t arg_length);

#endif
