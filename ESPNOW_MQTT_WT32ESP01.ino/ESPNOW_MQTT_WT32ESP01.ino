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
  This example uses FreeRTOS softwaretimers as there is no built-in Ticker library
*/
#include "defines.h"
#include <PubSubClient.h>
#include <esp_now.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

}
#include <AsyncMQTT_ESP32.h>
#include <AsyncHTTPRequest_Generic.h>
#include <AsyncHTTPRequest_Generic.hpp>


#define SENDTOPIC "ESPNow/key"
#define COMMANDTOPIC "ESPNow/command"
#define SERVICETOPIC "ESPNow/service"

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  int id;
  char state[7];
  float vBatt;
  float Temp;
  float Humid;
} struct_message;

// Create a struct_message called myData
struct_message myData;

volatile boolean haveReading = false;


#define MQTT_HOST         IPAddress(192, 168, 0, 255)
//#define MQTT_HOST         "broker.emqx.io"        // Broker address
#define MQTT_PORT         1883
#define MQTT_USERNAME "openhabforExample"
#define MQTT_PASSWORD "Your-Special-MQTT-Password-Here"
#define MQTT_CLIENT_ID "ESPNOW"

//const char *PubTopic  = "ESPNOW/";               // Topic to publish
String PubTopic = "ESPNOW/";

AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;

void connectToMqtt()

{
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void ETH_event(WiFiEvent_t event)
{
  switch (event)
  {
#if USING_CORE_ESP32_CORE_V200_PLUS

    case ARDUINO_EVENT_ETH_START:
      Serial.println("ETH starting");
      break;

    case ARDUINO_EVENT_ETH_CONNECTED:
      Serial.println("ETH connected");
      break;

    case ARDUINO_EVENT_ETH_GOT_IP:
      Serial.println("ETH got IP");
      Serial.print("IP address: ");
      Serial.println(ETH.localIP());
      connectToMqtt();
      break;

    case ARDUINO_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH lost connection");

      // ensure we don't reconnect to MQTT when no ETH
      xTimerStop(mqttReconnectTimer, 0);

      break;

    case ARDUINO_EVENT_ETH_STOP:
      Serial.println("ETH stops");

      // ensure we don't reconnect to MQTT when no ETH
      xTimerStop(mqttReconnectTimer, 0);

      break;
#else

    case SYSTEM_EVENT_ETH_CONNECTED:
      Serial.println(F("ETH Connected"));
      break;

    case SYSTEM_EVENT_ETH_GOT_IP:
      Serial.println("ETH connected");
      Serial.println("IP address: ");
      Serial.println(ETH.localIP());
      connectToMqtt();
      break;

    case SYSTEM_EVENT_ETH_DISCONNECTED:
    case SYSTEM_EVENT_ETH_STOP:
      Serial.println("ETH lost connection");

      // ensure we don't reconnect to MQTT when no ETH
      xTimerStop(mqttReconnectTimer, 0);

      break;
#endif

    default:
      break;
  }
}

void printSeparationLine()
{
  Serial.println("************************************************");
}

void onMqttConnect(bool sessionPresent)
{
  Serial.print("Connected to MQTT broker: ");
  Serial.print(MQTT_HOST);
  Serial.print(", port: ");
  Serial.println(MQTT_PORT);
  Serial.print("PubTopic: ");
  Serial.println(PubTopic);

  printSeparationLine();
  Serial.print("Session present: ");
  Serial.println(sessionPresent);

String PubTopic = "ESPNOW/startup";
uint16_t packetIdSub = mqttClient.subscribe(PubTopic.c_str(), 2);
Serial.print("Subscribing at QoS 2, packetId: ");
Serial.println(packetIdSub);

mqttClient.publish(PubTopic.c_str(), 0, true, "WT32_ETH01 Test ");
Serial.println("Publishing at QoS 0");

uint16_t packetIdPub1 = mqttClient.publish(PubTopic.c_str(), 1, true, "test 2");
Serial.print("Publishing at QoS 1, packetId: ");
Serial.println(packetIdPub1);

uint16_t packetIdPub2 = mqttClient.publish(PubTopic.c_str(), 1, true, "complete");
Serial.print("Publishing at QoS 2, packetId: ");
Serial.println(packetIdPub2);

  printSeparationLine();
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
  (void) reason;

  Serial.println("Disconnected from MQTT.");

  if (WT32_ETH01_isConnected())
  {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

void onMqttSubscribe(const uint16_t& packetId, const uint8_t& qos)
{
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(const uint16_t& packetId)
{
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void onMqttMessage(char* topic, char* payload, const AsyncMqttClientMessageProperties& properties,
                   const size_t& len, const size_t& index, const size_t& total)
{
  (void) payload;

  Serial.println("Publish received.");
  Serial.print("  topic: ");
  Serial.println(topic);
  Serial.print("  qos: ");
  Serial.println(properties.qos);
  Serial.print("  dup: ");
  Serial.println(properties.dup);
  Serial.print("  retain: ");
  Serial.println(properties.retain);
  Serial.print("  len: ");
  Serial.println(len);
  Serial.print("  index: ");
  Serial.println(index);
  Serial.print("  total: ");
  Serial.println(total);
}

void onMqttPublish(const uint16_t& packetId)
{
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void setup()
{
  Serial.begin(115200);

    //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);

  while (!Serial && millis() < 5000);

  Serial.print("\nStarting FullyFeature_WT32_ETH01 on ");
  Serial.print(ARDUINO_BOARD);
  Serial.println(" with " + String(SHIELD_TYPE));
  Serial.println(WEBSERVER_WT32_ETH01_VERSION);
  Serial.println(ASYNC_MQTT_ESP32_VERSION);

  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0,
                                    reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);

  mqttClient.setServer(MQTT_HOST, MQTT_PORT);

  //////////////////////////////////////////////

  // To be called before ETH.begin()
  WiFi.onEvent(ETH_event);

  //bool begin(uint8_t phy_addr=ETH_PHY_ADDR, int power=ETH_PHY_POWER, int mdc=ETH_PHY_MDC, int mdio=ETH_PHY_MDIO,
  //           eth_phy_type_t type=ETH_PHY_TYPE, eth_clock_mode_t clk_mode=ETH_CLK_MODE);
  //ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER, ETH_PHY_MDC, ETH_PHY_MDIO, ETH_PHY_TYPE, ETH_CLK_MODE);
  ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER);

  // Static IP, leave without this line to get IP via DHCP
  //bool config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns1 = 0, IPAddress dns2 = 0);
  //ETH.config(myIP, myGW, mySN, myDNS);

  WT32_ETH01_waitForConnect();

  //////////////////////////////////////////////
}

void loop(){
}
// callback function that will be executed when data is received
void OnDataRecv(const uint8_t* mac_addr, const uint8_t* incomingData, int len) {
  // Update the structures with the new incoming data
  memcpy(&myData, incomingData, sizeof(myData));
  int sensorId = myData.id;
  PubTopic += sensorId;

  // Create a DynamicJsonDocument object
  DynamicJsonDocument jsonDoc(1024);

  // Add the data to the jsonDoc object
  jsonDoc["id"] = myData.id;
  jsonDoc["state"] = String(myData.state);
  jsonDoc["vBatt"] = myData.vBatt;
  jsonDoc["Temp"] = myData.Temp;
  jsonDoc["Humid"] = myData.Humid;

  // Convert the jsonDoc object to a String
  String jsonString;
  serializeJson(jsonDoc, jsonString);

  int jsonStringLength = jsonString.length();
  char jsonData[jsonStringLength+1];
  jsonString.toCharArray(jsonData, jsonStringLength+1);
  String PubTopic = "ESPNOW/" + String(myData.id);
  mqttClient.publish(PubTopic.c_str(), 0, false, jsonData, jsonStringLength);}