//----------------------------------------------SMART WATER MANAGEMENT---------------------------------------------------------------

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

void callback(char* topic, byte* payload, unsigned int payloadLength);
const char* ssid = "SIRI JioFi";
const char* password = "bmr12345";

#include <Servo.h>
Servo myservo; 
//////////////////////
#define flow_pin D2  //gpio4
#define LED_PIN D7    //gpio13

volatile long flow_count=0;
float calibrationFactor = 4.5;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
float totalLitres;

unsigned long oldTime;

void ICACHE_RAM_ATTR flow_counter()
{
  flow_count++;
}




///////////////////////////
#define ORG "1lpuff"
#define DEVICE_TYPE "water555"
#define DEVICE_ID "555"
#define TOKEN "9876543210"

String data;
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char subtopic[] = "iot-2/cmd/tap/fmt/String";
char pubtopic[] = "iot-2/evt/flow/fmt/json";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;

WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);


void setup() 
{
  Serial.begin(115200);
  Serial.println();
  pinMode(D1,OUTPUT);
  wifiConnect();
  mqttConnect();
 myservo.attach(D5);

/////////////////
  
  flow_count        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;  
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);  // We have an active-low LED attached
 
  pinMode(flow_pin, INPUT);
  attachInterrupt(flow_pin, flow_counter, FALLING);
 
 //////////////////////////////////////
 
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

 
  ////////////
  
   if((millis() - oldTime) > 1000)    // Only process counters once per second
  {
    detachInterrupt(flow_pin);
    flowRate = ((1000.0 / (millis() - oldTime)) * flow_count) / calibrationFactor;
    oldTime = millis();
    flowMilliLitres = (flowRate / 60) * 1000;
    totalMilliLitres += flowMilliLitres;     
    totalLitres = totalMilliLitres * 0.001;
    unsigned int frac;   
    Serial.print("flowrate: ");
    Serial.print(int(flowRate));  // Print the integer part of the variable
    Serial.println("L/min");
   
    /*Serial.print(".");             // Print the decimal point
    frac = (flowRate - int(flowRate)) * 10;
    Serial.print(frac, DEC) ;      // Print the fractional part of the variable
    Serial.print("L/min");
    
    Serial.print("Current Liquid Flowing: ");             // Output separator
    Serial.print(flowMilliLitres);
    Serial.print("mL/Sec");
    Serial.print("  Output Liquid Quantity: ");             // Output separator
    Serial.print(totalLitres);
    Serial.println("L");*/
 
    flow_count = 0;

    attachInterrupt(flow_pin, flow_counter, FALLING);
  }
  
  
  ///////////
  if(flowRate > 0.2)
  {
  PublishData(flowRate);
  delay(2000);
  }
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

void callback(char* subtopic, byte* payload, unsigned int payloadLength)
{
  Serial.print("callback invoked for topic: "); 
  Serial.println(subtopic);

  for (int i = 0; i < payloadLength; i++) 
  {
    data += (char)payload[i];
  }
    Serial.println(data);
    if(data == "tap_on")
    {
      // digitalWrite(D1,HIGH);
       myservo.write(90);              // tell servo to go to position in variable 'pos'
    delay(15);
      Serial.println("tap is on");
    }
    else if(data == "tap_off")
    {
        //digitalWrite(D1,LOW);
        Serial.println("tap is off");
         myservo.write(0);              // tell servo to go to position in variable 'pos'
    delay(15);
    }
    data ="";
}
void PublishData(float flowRate)
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
  String payload = "{\"d\":{\"house1_flowRate\":";
  payload += flowRate;
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
