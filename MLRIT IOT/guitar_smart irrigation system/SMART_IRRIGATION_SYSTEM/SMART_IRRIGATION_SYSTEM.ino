const int sensor_pin = A0;

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

void callback(char* topic, byte* payload, unsigned int payloadLength);
const char* ssid = "VASU GUITAR";
const char* password = "VASU2660";


#include "DHT.h"
#define DHTPIN D2   
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);


#define ORG "ti62vq"
#define DEVICE_TYPE "nodemcu555"
#define DEVICE_ID "5555"
#define TOKEN "55555555"

String data;
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char subtopic[] = "iot-2/cmd/home/fmt/String";
char pubtopic[] = "iot-2/evt/dht11/fmt/json";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;

WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);

float moisture_percentage;
void setup() 
{
  Serial.begin(9600);
  Serial.println();
  wifiConnect();
  mqttConnect();

  Serial.println("DHTxx test!");
  dht.begin();
   Serial.begin(9600);
  pinMode(D6, OUTPUT); 
  pinMode(D7, OUTPUT);  
  pinMode(D4, OUTPUT);

  
 Serial.print("Connecting to ");
 Serial.print(ssid);
 WiFi.begin(ssid, password);
 while (WiFi.status()!= WL_CONNECTED) 
 {
      delay(500);
      Serial.print(".");
 } 
 Serial.println("");
 
 Serial.print("WiFi connected, IP address : ");
 Serial.println(WiFi.localIP());
}

void loop() 
{
  if (!client.loop()) 
  {
    mqttConnect();
  }
  delay(100);

  float moisture_percentage;
  moisture_percentage = ( 100.00 - ( (analogRead(sensor_pin)/1023.00) * 100.00 ) );
  Serial.print("Soil Moisture(in Percentage) = ");
  Serial.print(moisture_percentage);
  Serial.println("%");
  if(moisture_percentage<=30.00)
   digitalWrite(D4,HIGH);
  else if(moisture_percentage>30.00)
      digitalWrite(D4,LOW);
  
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  if (isnan(h) || isnan(t) || isnan(f))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C ");
 

  digitalWrite(D6,LOW);
  digitalWrite(D7,HIGH);

  PublishData(t,h,moisture_percentage);
  delay(1000);
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
  Serial.print("nWiFi connected, IP address: "); Serial.println(WiFi.localIP());
}

void mqttConnect() 
{
  if (!client.connected())
  {
    Serial.print("Reconnecting MQTT client to ");
    Serial.println(server);
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

void callback(char* topic, byte* payload, unsigned int payloadLength)
{
  Serial.print("callback invoked for topic: "); 
  Serial.println(subtopic);

  for (int i = 0; i < payloadLength; i++) 
  {
    data += (char)payload[i];
  }
    Serial.println(data);
    if(data == "light_ON")
    {
      digitalWrite(D1,HIGH);
      Serial.println("Light is Switched ON");
    }
    else if(data == "light_OFF")
    {
       digitalWrite(D1,LOW);
        Serial.println("Light is Switched OFF");
    }
    data ="";
}
void PublishData(float temp, float humid,float m)
{
  if (!client.connected()) 
  {
    Serial.print("Reconnecting client to ");
    Serial.println(server);
    while (!client.connect(clientId, authMethod, token)) 
    {
      Serial.print(".");
      delay(500);
    }
     Serial.println();
  }
  String payload = "{\"d\":{\"temperature\":";
  payload += temp;
  payload+="," "\"humidity\":";
  payload += humid;
  payload+="," "\"soil_moisture\":";
  payload += m;
  
  payload += "}}";
 Serial.print("Sending payload: ");
 Serial.println(payload);
  
 if (client.publish(pubtopic, (char*) payload.c_str()))
 {
    Serial.println("Publish ok");
 }
 else 
 {
 Serial.println("Publish failed");
 }
}
