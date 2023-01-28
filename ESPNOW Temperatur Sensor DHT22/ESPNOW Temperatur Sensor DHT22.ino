#include "DHT.h"
#include<ESP8266WiFi.h>
#include<espnow.h>

uint8_t bridgeAddress1[] = {0xC4, 0xDE, 0xE2, 0xB2, 0x47, 0x90};   //please update this with the MAC address of your ESP-NOW TO MQTT brigde
//uint8_t bridgeAddress2[] = {0xA1, 0xBC, 0xD2, 0xE3, 0x45, 0x67};   //please update this with the MAC address of your ESP-NOW TO MQTT brigde

// Set your Board ID (ESP32 Sender #1 = BOARD_ID 1, ESP32 Sender #2 = BOARD_ID 2, etc)
#define BOARD_ID 20

// define DHT Type
#define DHT_TYPE DHT22
  
const int DHT_PIN = 5; 
  
DHT dht(DHT_PIN, DHT_TYPE);

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  int id;
  char state[7];
  float vBatt;      //i changed this typedef to float, to get decimal Numbers
  float Temp;
  float Humid;
} struct_message;

// Create a struct_message called test to store variables to be sent
struct_message myData;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  /* Serial.print("\r\nLast Packet Send Status: ");
    if (sendStatus == 0){
     Serial.println("Delivery success");
    }
    else{
     Serial.println("Delivery fail");
    }
  */
}

void setup() {
  dht.begin();
  //Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    //Serial.println("Error initializing ESP-NOW");
    return;
  }
  // Set ESP-NOW role
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

  // Once ESPNow is successfully init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  esp_now_add_peer(bridgeAddress1, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  //esp_now_add_peer(bridgeAddress2, ESP_NOW_ROLE_SLAVE, 2, NULL, 0);
}

// runs over and over again 

void loop() {
myData.id = BOARD_ID;  
float h = dht.readHumidity();
float t = dht.readTemperature();

myData.vBatt = (analogRead(A0) * 3.6 / 1023);

myData.Temp = dht.readTemperature();
myData.Humid = dht.readHumidity();

// Send message via ESP-NOW
esp_now_send(0, (uint8_t *) &myData, sizeof(myData));
ESP.deepSleep(30e8);
delay(100);
}
