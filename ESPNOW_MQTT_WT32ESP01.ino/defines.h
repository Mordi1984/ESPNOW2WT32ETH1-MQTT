/****************************************************************************************************************************
  ESPNOW_MQTT_WT32ESP01.ino

  AsyncMQTT_ESP32 is a library for ESP32 boards using WiFi or LwIP W5500, LAN8720 or ENC28J60

  Based on and modified from :

  1) async-mqtt-client (https://github.com/marvinroger/async-mqtt-client)
  2) async-mqtt-client (https://github.com/khoih-prog/AsyncMQTT_Generic)

  Built by Khoi Hoang https://github.com/khoih-prog/AsyncMQTT_ESP32

  Modified by Mordi1984 (https://github.com/Mordi1984/ESPNOW2WT32ETH1-MQTT/)
  I that Code to work with "ESP-NOW" from (https://github.com/espressif/esp-idf/blob/master/components/esp_wifi/include/esp_now.h)

  Published MQTT Message looks like: 
  ESPNOW/1 //The Board-ID is the Topic - Here Board-ID 1
  MQTT Message looks like this:
  {"id":1,"state":"Open","vBatt":3.482795715,"Temp":0,"Humid":0}
  {"id":1,"state":"Closed","vBatt":3.482795715,"Temp":0,"Humid":0}

   *****************************************************************************************************************************/
/*
#ifndef defines_h
#define defines_h

// Debug Level from 0 to 4
#define _ETHERNET_WEBSERVER_LOGLEVEL_       1
#define _ASYNC_MQTT_LOGLEVEL_               1

#include <WebServer_WT32_ETH01.h>

// Select the IP address according to your local network
IPAddress myIP(192, 168, 0, 2); //Backup -> Not needes exactly if you use DHCP
IPAddress myGW(192, 168, 0, 1);
IPAddress mySN(255, 255, 255, 0);


// Google DNS Server IP
IPAddress myDNS(8, 8, 8, 8);

#endif    //defines_h