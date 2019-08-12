#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
 
//-------- Customise these values -----------
const char* ssid = "MLRITM@DGL";
const char* password = "digi@123";
#define ONE_WIRE_BUS D2
OneWire oneWire(ONE_WIRE_BUS);


#define ORG "rwcoys"
#define DEVICE_TYPE "NODEMCU5060"
#define DEVICE_ID "DELL"
#define TOKEN "9381575060"
//-------- Customise the above values --------
 
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char topic[] = "iot-2/evt/anil/fmt/json";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;

float temp;
WiFiClient wifiClient;
PubSubClient client(server, 1883,wifiClient);
DallasTemperature sensors(&oneWire);

/// turbidity
int ledPin = 13; 
int photocellPin = A0; 
float NTUval;
float val;

void setup() {
 Serial.begin(9600);
 sensors.begin();

 // turbidity
 pinMode (ledPin, OUTPUT);  //Set pin to outut
 pinMode (photocellPin, INPUT);
 
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
}
 
void loop(void) {
  
sensors.requestTemperatures(); 
Serial.print("Celsius temperature: ");
Serial.println(sensors.getTempCByIndex(0));

temp = sensors.getTempCByIndex(0);
delay(1000);

// turbidity
val = analogRead(photocellPin);
Serial.print("val: ");
Serial.println(val);
digitalWrite(13, HIGH);
float ardval = val*0.00488758553;
NTUval = ardval;  //200 = units in NTU
Serial.print("NTUval ");
Serial.println(NTUval);
delay(1000);

// publish
PublishData(temp,NTUval);
}



void PublishData(float temp,float NTUval){
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
  payload+="," "\"turbidity\":";
  payload += NTUval;
  payload += "}}";
 Serial.print("Sending payload: ");
 Serial.println(payload);
  
 if (client.publish(topic, (char*) payload.c_str())) {
 Serial.println("Publish ok");
 } else {
 Serial.println("Publish failed");
 }
}
