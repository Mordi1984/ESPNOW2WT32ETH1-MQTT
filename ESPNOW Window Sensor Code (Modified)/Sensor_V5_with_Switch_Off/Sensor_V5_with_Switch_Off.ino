// Big respect to:
// https://github.com/MakerMeik
// https://github.com/gadjet/Window-Door-sensor-Version-5 - Original Code is from him!
// I changed only the vBatt typedef and removed an Multiplicator
// This is i fact not my Work.


#include<ESP8266WiFi.h>
#include<espnow.h>


uint8_t bridgeAddress1[] = {0xA1, 0xBC, 0xD2, 0xE3, 0x45, 0x67};   //please update this with the MAC address of your ESP-NOW TO MQTT brigde
//uint8_t bridgeAddress2[] = {0xA1, 0xBC, 0xD2, 0xE3, 0x45, 0x67};   //please update this with the MAC address of your ESP-NOW TO MQTT brigde

// Set your Board ID (ESP32 Sender #1 = BOARD_ID 1, ESP32 Sender #2 = BOARD_ID 2, etc)
#define BOARD_ID 2

const int reedSwitch = 13;
const int powerOff = 16; // set to low to turn off LDO

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  int id;
  char state[7];
  float vBatt;      /i changed this to an def to get decimal Numbers
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
  // Init Serial Monitor
  //Serial.begin(115200);
  // initialize the reed switch pin as an input:
  pinMode(reedSwitch, INPUT);
  // initialize the wakeup pin as an input:
  pinMode(powerOff, OUTPUT);
  digitalWrite(powerOff, HIGH);
  // Set device as a Wi-Fi Station
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

void loop() {
  // Set values to send
  myData.id = BOARD_ID;
  // Read the state of the reed switch and send open or closed
  if (digitalRead(reedSwitch) == HIGH) {
    strcpy(myData.state, "Open");
  }
  else {
    strcpy(myData.state, "Closed");
  }

  myData.vBatt = (analogRead(A0) * 4.1 / 1023);   // i removed the multiplikator to get an decimal Number instead an ten times higher Value
#ifdef useTempDS18B20
  myData.Temp = sensors.getTempC(Thermometer);
#elseif #elseif useAHT10 || seBME280
  myData.Temp = sensors.getTempC(Thermometer);
  myData.Humid = 0.0; //sensors.getTempC(Thermometer);
#endif

  // Send message via ESP-NOW
  esp_now_send(0, (uint8_t *) &myData, sizeof(myData));
  //ESP.deepSleep(0);
  delay(100);
  digitalWrite(powerOff, LOW); //Switch off supply
}
