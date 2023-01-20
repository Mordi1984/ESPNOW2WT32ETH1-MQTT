# ESPNOW2WT32ETH1-MQTT
ESPNOW Receiver and ESPNOW to MQTT Published over Ethernet (WT32-ETH1)

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

  The Sender Code was original from:
  https://github.com/gadjet/Window-Door-sensor-Version-5
  
  I changed only changed the Voltage calculation and also the ESP-NOW Struct message to transmit decimal Values.
  
  
  If you put it to Openhab, all Values can be readed with the topic i wrote down below.
      
      UID: mqtt:topic:1234567:1234567
label: Contact
thingTypeUID: mqtt:topic
configuration: {}
bridgeUID: mqtt:broker:1234567
location: Window
channels:
  - id: Contact
    channelTypeUID: mqtt:contact
    label: Windowcontact
    description: ""
    configuration:
      postCommand: true
      stateTopic: ESPNOW/1
      transformationPattern: JSONPATH:$.state
      off: Closed
      on: Open
  - id: UID: mqtt:topic:f16d1d9413:90aa4fd327
label: Windowcontact
thingTypeUID: mqtt:topic
configuration: {}
bridgeUID: mqtt:broker:f16d1d9413
location: Window
channels:
  - id: Windowcontactstate
    channelTypeUID: mqtt:contact
    label: Windowcontactstate
    description: ""
    configuration:
      postCommand: true
      stateTopic: ESPNOW/1
      transformationPattern: JSONPATH:$.state
      off: Closed
      on: Open
  - id: Windowcontact_battery
    channelTypeUID: mqtt:number
    label: Fensterkontakt Windowcontact Battery
    description: ""
    configuration:
      unit: '"V"'
      min: // Your Minimum Voltage
      formatBeforePublish: "%3f"
      max: //Your Max Voltage
      stateTopic: ESPNOW/1
      transformationPattern: JSONPATH:$.vBatt
  - id: Windowcontact_ESPNOW_ID
    channelTypeUID: mqtt:number
    label: Windowcontact ESPNOW-ID
    description: ""
    configuration:
      stateTopic: ESPNOW/1
      transformationPattern: JSONPATH:$.id
