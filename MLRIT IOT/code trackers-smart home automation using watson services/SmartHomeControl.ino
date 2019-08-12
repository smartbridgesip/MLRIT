#include <ESP8266WiFi.h>
#include <PubSubClient.h>

void callback(char* subtopic, byte* payload, unsigned int payloadLength);
const char* ssid = "sathwik";
const char* password = "87654321";

#define ORG "7qjod5"
#define DEVICE_TYPE "home"
#define DEVICE_ID "1234"
#define TOKEN "12345678"
String data;
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char subtopic[] = "iot-2/cmd/home/fmt/String";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;
/////////////////////////////////////////////////////////

WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);

#define led D1
void setup() 
{
  Serial.begin(115200);
  Serial.println();
  pinMode(led,OUTPUT);
  wifiConnect();
  mqttConnect();
}

void loop() 
{
  if (!client.loop()) 
  {
    mqttConnect();
  }
delay(100);
}
void wifiConnect()
{
 Serial.print("Connecting to "); Serial.print(ssid);
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) 
 {
 delay(500);
 Serial.print(".");
 }
 Serial.print("WiFi connected, IP address: "); Serial.println(WiFi.localIP());
}


void mqttConnect()
{
if (!client.connected())
{
Serial.print("Reconnecting MQTT client to "); Serial.println(server);
while (!client.connect(clientId, authMethod, token)) 
{
  Serial.print(".");
  delay(500);
}
initManagedDevice();
Serial.println();
}
}


void initManagedDevice() 
{
if (client.subscribe(subtopic)) 
{
 Serial.println("subscribe to cmd OK");
} 
else 
{
Serial.println("subscribe to cmd FAILED");
}
}

void callback(char* subtopic, byte* payload, unsigned int payloadLength) 
{
Serial.print("callback invoked for topic: ");
Serial.println(subtopic);
for (int i = 0; i < payloadLength; i++) 
  {
    data += (char)payload[i];
  }
Serial.println(data);
if(data == "lighton")
{
  digitalWrite(led,HIGH);
  Serial.println("Light is Switched ON");
}
else if(data == "lightoff")
{
digitalWrite(led,LOW);
Serial.println("Light is Switched OFF");
}
data ="";
}
