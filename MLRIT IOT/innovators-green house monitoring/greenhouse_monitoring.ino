#include <ESP8266WiFi.h>
#include <PubSubClient.h>
 int mq135value;
int mq2value;

//-------- Customise these values -----------
const char* ssid = "MLRITM@DGL";
const char* password = "digi@123";

//////////////////////////

#include "DHT.h"
#define DHTPIN D2    // what pin we're connected to
#define DHTTYPE DHT11   // define type of sensor DHT 11
DHT dht (DHTPIN, DHTTYPE);

 ///////////////////////
 
#define ORG "xm610m"
#define DEVICE_TYPE "nodemcu0404"
#define DEVICE_ID "0071"
#define TOKEN "9381228129"

//-------- Customise the above values --------
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char topic[] = "iot-2/evt/madhuri/fmt/json";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;
 
WiFiClient wifiClient;
PubSubClient client(server, 1883,wifiClient);

void setup() {
 Serial.begin(115200);
 Serial.println();
 dht.begin();
 pinMode(D0, OUTPUT); 
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
}
 
void loop() {
  mq2value = analogRead(A0); // o-1
mq135value = digitalRead(D0); // 0 or 1024


Serial.println("mq2value" + String(mq2value));
Serial.println("mq135value" + String(mq135value));
float h = dht.readHumidity();
float t = dht.readTemperature();

if (isnan(h) || isnan(t))
{
Serial.println("Failed to read from DHT sensor!");
delay(1000);
return;
}
PublishData(t,h,mq2value,mq135value);
delay(1000);
}



void PublishData(float temp, float humid,int mq2,int mq135){
 if (!!!client.connected()) {
 Serial.print("Reconnecting client to ");
 Serial.println(server);
 while (!!!client.connect(clientId, authMethod, token)) {
 Serial.print(".");
 delay(500);
 }
 Serial.println();
 }
  String payload = "{\"d\":{\"temperature\":";
  payload += temp;
  payload+="," "\"humidity\":";
  payload += humid;
  payload+="," "\"mq2\":";
  payload += mq2;
  payload+="," "\"mq135\":";
  payload += mq135;
  payload += "}}";
 Serial.print("Sending payload: ");
 Serial.println(payload);
  
 if (client.publish(topic, (char*) payload.c_str())) {
 Serial.println("Publish ok");
 } else {
 Serial.println("Publish failed");
 }
}
