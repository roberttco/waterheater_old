#include <Arduino.h>
#include <Ethernet.h>

#include "NetworkManagement.h"
#include "MyEEPROM.h"

byte mac[6] = { 0xE3,0xA8,0xD7,0xE2,0xF3,0x7C };
byte ip[4] = {0,0,0,0};

// ethernet connection variables
byte connectionStatus = NETWORK_DISCONNECTED;
bool networkInitialized = false;
EthernetLinkStatus lastLinkStatus = Unknown;
byte dhcpRetries = 3;

EthernetServer server = NULL;

EthernetServer getServer()
{
  return server;
}

EthernetClient getClient()
{
  return server.available();
}

bool networkOk()
{
  return networkInitialized;
}

bool networkConnected()
{
  return (connectionStatus != NETWORK_DISCONNECTED);
}

void getIpAddress(byte *ip1, byte *ip2, byte *ip3, byte *ip4, bool *dhcp)
{
  *ip1 = ip[0];
  *ip2 = ip[1];
  *ip3 = ip[2];
  *ip4 = ip[3];

  *dhcp = (connectionStatus == NETWORK_DHCP);
}

IPAddress getIpAddress()
{
  return IPAddress(ip[0],ip[1],ip[2],ip[3]);
}

bool setIpAddress(byte ip1, byte ip2, byte ip3, byte ip4)
{
  if (ip4 > 0 && ip4 < 255)
  {
    ip[0] = ip1;
    ip[1] = ip2;
    ip[2] = ip3;
    ip[3] = ip4;
    return true;
  }
  return false;
}

void saveIpAddress()
{
  for (int i = 0;i < 4;i++) { EEPROM.put(IP_ADDRESS+i,ip[i]); delay(5); }
}

bool initNetwork()
{
  // first things first - get the address from EEPROM - the single point of truth
  // if the value in EEPROM is bogus, then  write a 0 there.
  NMDEBUG_PRINT(F("Initializing network.\r\nGetting IP from EEPROM - "));
  
  for (uint8_t i = 0; i < 4; i++)
  {
    EEPROM.get(IP_ADDRESS + i,ip[i]);
    
    if (isnan(ip[i])) 
    { 
      ip[i] = 0;
      EEPROM.put(IP_ADDRESS+i,0);
    }
  }

  NMDEBUG_PRINTLN(getIpAddress());

  server = EthernetServer(80);  // create a server at port 80

  networkInitialized = true;
  return true;
}

bool connectToNetwork(bool reconnect = false)
{
  if ((networkInitialized == false) || ((connectionStatus != NETWORK_DISCONNECTED) && (reconnect == false)))
  {
    return false;
  }
  else
  {
    NMDEBUG_PRINTLN("");
    NMDEBUG_PRINTLN(F("Initializing NIC"));

    if (ip[0] == 0) // if the first octet is a 0 then use DHCP
    {
#ifndef ALWAYS_USE_DEFAULT_IP
      uint8_t dhcpRetryCount = MAX_DHCP_RETRIES; // after this many retries, assing the default address.
      while ((dhcpRetryCount) > 0 && (connectionStatus == NETWORK_DISCONNECTED))
      {
        Serial.println(F("Trying DHCP"));
        connectionStatus = (Ethernet.begin(mac) == 1 ? NETWORK_DHCP : NETWORK_DISCONNECTED);
        dhcpRetryCount--;
      }
#endif
      if (connectionStatus == NETWORK_DISCONNECTED)
      {
        NMDEBUG_PRINTLN(F("Using default IP"));
        Ethernet.begin(mac, IPAddress(172,54,0,2));
        connectionStatus = NETWORK_STATIC;
        setIpAddress(172,54,0,2);
      }
    }
    else
    {
      NMDEBUG_PRINTLN(F("Trying STATIC"));
      Ethernet.begin(mac, getIpAddress());
      connectionStatus = NETWORK_STATIC;
    }
  }

  if (connectionStatus == NETWORK_DISCONNECTED)
  {
    Serial.println(F("ETH connect failed"));
  }
  else if (Ethernet.linkStatus() == LinkOFF)
  {
    NMDEBUG_PRINTLN(F("No ETH link"));
    connectionStatus = NETWORK_DISCONNECTED;
  }
  else
  {
    Serial.println(Ethernet.localIP());
    server.begin();           // start to listen for clients
  }

  return networkConnected();
}

void renewDhcpLease()
{
  // renew DHCP lease if needed - https://www.arduino.cc/en/Reference/EthernetMaintain
  if (connectionStatus == NETWORK_DHCP)
  {
    byte renewStatus = Ethernet.maintain();
    if (renewStatus == 1 || renewStatus == 3) 
    {
      connectionStatus = NETWORK_DISCONNECTED;
      dhcpRetries = 3;
    }
  }
}

// return true if the cable was plugged in
bool checkNetworkCablePlugin()
{
  bool rval = false;
  
  EthernetLinkStatus linkStatus = Ethernet.linkStatus();

  if (lastLinkStatus == LinkOFF && linkStatus == LinkON)  // cable plug-in
  {
    NMDEBUG_PRINTLN(F("Cable plugged in"));
    if (initNetwork())
    {
      connectToNetwork(false);
      rval = true;
    }
  }
  else if (lastLinkStatus == LinkON && linkStatus == LinkOFF) // cable unplug
  {
    NMDEBUG_PRINTLN(F("Cable unplugged"));
    networkInitialized = false;
    connectionStatus = NETWORK_DISCONNECTED;
  }

  lastLinkStatus = linkStatus;
  return rval;
}
