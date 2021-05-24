#ifndef __WEBSERVER_H_
#define __WEBSERVER_H_

//#define WSDEBUG

#ifdef WSDEBUG
#define WSDEBUG_PRINT(x) Serial.print(x)
#define WSDEBUG_PRINTLN(x) Serial.println(x)
#else
#define WSDEBUG_PRINT(x) 
#define WSDEBUG_PRINTLN(X)
#endif

bool doWebServer();

#endif
