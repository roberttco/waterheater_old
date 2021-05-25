#ifndef __NETWORKMANAGEMENT_H__
#define __NETWORKMANAGEMENT_H__

#include <Ethernet.h>

//#define NMDEBUG

#ifdef NMDEBUG
#define NMDEBUG_PRINT(x) Serial.print(x)
#define NMDEBUG_PRINTLN(x) Serial.println(x)
#else
#define NMDEBUG_PRINT(x) 
#define NMDEBUG_PRINTLN(X)
#endif

#define MAX_DHCP_RETRIES    3

//#define ALWAYS_USE_DEFAULT_IP

// ethernet connection variables
#define NETWORK_DISCONNECTED 0
#define NETWORK_DHCP 1
#define NETWORK_STATIC 2

bool connectToNetwork(bool reconnect);
bool networkConnected();
void renewDhcpLease();
void saveIpAddress();
void setIPAddress(IPAddress address);
void getIpAddress(byte *ip1, byte *ip2, byte *ip3, byte *ip4, bool *dhcp);
IPAddress getIpAddress();
EthernetServer getServer();
EthernetClient getClient();
bool setIpAddress(byte ip1, byte ip2, byte ip3, byte ip4);
bool initNetwork();
bool networkOk();
bool checkNetworkCablePlugin();

#endif
