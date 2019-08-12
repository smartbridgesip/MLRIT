#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS1307.h>
RtcDS1307<TwoWire> Rtc(Wire);
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

char Hou;
char Min;

const char* ssid = "MLRITM@DGL";
const char* password = "digi@123";
String command,data;

#define ORG "fwtttv"
#define DEVICE_TYPE "Reminder-2019"
#define DEVICE_ID "nodemcu_reminder"
#define TOKEN "123456789"
#define led D3
#define buzzer D4

char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char pubtopic[] = "iot-2/evt/reminder/fmt/json";
char subtopic[] = "iot-2/evt/reminder/fmt/json";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;

WiFiClient wifiClient;
PubSubClient client(server, 1883,wifiClient);

void setup () 
{
    Serial.begin(57600);
    Serial.print("compiled: ");
    Serial.print(__DATE__);
    Serial.println(__TIME__);
    pinMode (led,OUTPUT);
    pinMode (buzzer,OUTPUT);

  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
  delay(500);
  Serial.print(".");
  } 
  Serial.println("");
 
  Serial.print("WiFi connected, IP address: ");
  Serial.println(WiFi.localIP());

    //--------RTC SETUP ------------
    // if you are using ESP-01 then uncomment the line below to reset the pins to
    // the available pins for SDA, SCL
    // Wire.begin(0, 2); // due to limited pins, use pin 0 and 2 for SDA, SCL
    
    Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    printDateTime(compiled);
    Serial.println();

    if (!Rtc.IsDateTimeValid()) 
    {
        if (Rtc.LastError() != 0)
        {
            Serial.print("RTC communications error = ");
            Serial.println(Rtc.LastError());
        }
        else
        {
            Serial.println("RTC lost confidence in the DateTime!");
            Rtc.SetDateTime(compiled);
        }
    }

    if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) 
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }
    else if (now > compiled) 
    {
        Serial.println("RTC is newer than compile time. (this is expected)");
    }
    else if (now == compiled) 
    {
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }

    Rtc.SetSquareWavePin(DS1307SquareWaveOut_Low); 
    wifiConnect();//user define function
  mqttConnect();// user define function

}
    
void loop () 
{
    if (isnan(Hou) || isnan(Min))
      {
        Serial.println("Failed to read from DHT sensor!");
        delay(1000);
        return;
      }
    
    if (!client.loop())
      {
          mqttConnect();
      }
      PublishData(Hou,Min);
    delay(100);
    if (!Rtc.IsDateTimeValid()) 
    {
        if (Rtc.LastError() != 0)
        {
            Serial.print("RTC communications error = ");
            Serial.println(Rtc.LastError());
        }
        else
        {
            Serial.println("RTC lost confidence in the DateTime!");
        }
    }

    RtcDateTime now = Rtc.GetDateTime();

    printDateTime(now);
    Serial.println();

    delay(5000); // five seconds
}

void wifiConnect() {
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.print("WiFi connected, IP address: "); 
  Serial.println(WiFi.localIP());
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


#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
{
    char Hou[20];
    char Min[20];

    snprintf_P(Hou, countof(Hou),PSTR("%02u"),dt.Hour());
    snprintf_P(Min, countof(Min),PSTR("%02u"),dt.Minute());
    Serial.println(Hou);
    Serial.println(Min);
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

void PublishData(char Hou, char Min)
{
 if (!!!client.connected()) 
 {
 Serial.print("Reconnecting client to ");
 Serial.println(server);
 while (!!!client.connect(clientId, authMethod, token)) 
 {
 Serial.print(".");
 delay(500);
 }
 Serial.println();
 }
  String payload = "{\"u\":{\"Hours\":";
  payload += Hou;
  payload+="," "\"minutes\":";
  payload += Min;
  payload += "}}";
 Serial.print("Sending payload: ");
 Serial.println(payload);
  
 if (client.publish(subtopic, (char*) payload.c_str())) 
 {
 Serial.println("Publish ok");
 }
 else 
 {
 Serial.println("Publish failed");
 }
}

void callback(char Hou, char Min, byte* payload, unsigned int payloadLength) 
{
  Serial.print("callback invoked for topic: "); 
  Serial.println(subtopic);

for (int i = 0; i < payloadLength; i++) 
{
Serial.println((char)payload[i]);
 command += (char)payload[i];
}
Serial.println(command);
if(command == "")
{
digitalWrite(led,HIGH);
tone (buzzer,100);
 Serial.println("Light is Switched ON");
Serial.println ("Buzzer in ON");
}
if(command == "")
{
digitalWrite(led,LOW);
 noTone (buzzer);
Serial.println ("Light is Switched OFF");
Serial.println ("Buzzer is OFF");
}
command ="";
}
