#include <ESP8266WiFi.h>
#include <PubSubClient.h>

void callback(char* subtopic, byte* payload, unsigned int payloadLength);
const char* ssid = "MLRITM@DGL";
const char* password = "digi@123";
int ldr;
 
#define ORG "dooo8z"
#define DEVICE_TYPE "nodemcu"
#define DEVICE_ID "0404"
#define TOKEN "7730039327"
String command;
//-------- Customise the above values --------
 
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char subtopic[] = "iot-2/cmd/manual control/fmt/String";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;
 
WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);
void setup() {
 Serial.begin(115200);
 Serial.println();
 Serial.print("Connecting to ");
 Serial.print(ssid);
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.print(".");
 } 
 Serial.println("");
 
 Serial.print("WiFi connected, IP address: ");
 Serial.println(WiFi.localIP());
 pinMode(D1,OUTPUT);
 pinMode(D2,OUTPUT);
 pinMode(D3,OUTPUT);
  wifiConnect();
  mqttConnect();
}
void loop() {
  if (!client.loop()) {
    mqttConnect();
    
  }
delay(100);

 
}
void wifiConnect() {
  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("nWiFi connected, IP address: "); Serial.println(WiFi.localIP());
}

void mqttConnect() {
  if (!client.connected()) {
    Serial.print("Reconnecting MQTT client to "); Serial.println(server);
    while (!client.connect(clientId, authMethod, token)) {
      Serial.print(".");
      delay(500);
    }
    initManagedDevice();
    Serial.println();
  }
}
void initManagedDevice() {
  if (client.subscribe(subtopic)) {
    Serial.println("subscribe to cmd OK");
  } else {
    Serial.println("subscribe to cmd FAILED");
  }
}

void callback(char* subtopic, byte* payload, unsigned int payloadLength) {
  Serial.print("callback invoked for topic: "); Serial.println(subtopic);

  for (int i = 0; i < payloadLength; i++) {
    //Serial.println((char)payload[i]);
    command += (char)payload[i];
  }
Serial.println(command);
if(command == "LIGHTON"){
  digitalWrite(D1,HIGH);
  digitalWrite(D2,HIGH);
  digitalWrite(D3,HIGH);
  Serial.println("Light is Switched ON");
}
else if(command == "LIGHTOFF"){
  digitalWrite(D1,LOW);
  digitalWrite(D2,LOW);
  digitalWrite(D3,LOW);
  Serial.println("Light is Switched OFF");
}
else if(command=="automatic"){
  ldr=analogRead(A0);
  Serial.print("LDR value:");
  Serial.println(ldr);
  int mv=map(ldr,0,1024,0,255);
  Serial.print("mapped ldr value:");
  Serial.print(mv);
  analogWrite(D1,mv);
  analogWrite(D2,mv);
  analogWrite(D3,mv);
  delay(500);
}
 
command ="";
}
