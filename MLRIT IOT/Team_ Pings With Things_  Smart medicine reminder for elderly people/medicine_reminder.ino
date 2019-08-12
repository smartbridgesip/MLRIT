
#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS1307.h>
RtcDS1307<TwoWire> Rtc(Wire);
#include <ESP8266WiFi.h>
#include <PubSubClient.h>////mqtt protocol

void callback(char* subtopic, byte* payload, unsigned int payloadLength);
///network credentials
const char* ssid = "MLRITM@DGL";
const char* password = "digi@123";

///device credentials
#define ORG "fwtttv"
#define DEVICE_TYPE "Reminder-2019"
#define DEVICE_ID "nodemcu_reminder"
#define TOKEN "123456789"

String data;
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char subtopic[] = "iot-2/cmd/reminder/fmt/String";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;
//Serial.println(clientID);

///////////////////////////////////////////////////////////
WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);// call back is a inbuilt function

#define led D1
#define buzzer D2
void setup()
{
  Serial.begin(57600);
  Serial.print("compiled: ");
  Serial.print(__DATE__);
  Serial.println(__TIME__);

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

  Serial.begin(57600);
  Serial.println();
  pinMode(led, OUTPUT);
  pinMode (buzzer, OUTPUT);
  wifiConnect();//user define function
  mqttConnect();// user define function
}

void loop()
{
 // int hours = rs();
  int m = Rtc.GetTimeMinutes();
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

  if (!client.loop())
  {
    mqttConnect();
  }
  delay(100);
}

void wifiConnect()
{
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
    Serial.print("Reconnecting MQTT client to ");
    Serial.println(server);
    while (!client.connect(clientId, authMethod, token))
    {
      Serial.print(".");
      delay(500);
    }
    initManagedDevice();//user defined function
    Serial.println();
  }
}
void initManagedDevice()
{
  if (client.subscribe(subtopic))
  {
    Serial.println("subscribe to command OK");
  }
  else
  {
    Serial.println("subscribe to command FAILED");
  }
}

void callback(char* subtopic, byte* payload, unsigned int payloadLength)
{
  Serial.print("callback involed for topic: ");
  Serial.println(subtopic);

  for (int i = 0; i < payloadLength; i++)
  {
    //Serial.println((char)payload[i]);
    data += (char)payload[i];// data = data +(char)payload[i]
  }
  Serial.println(data);//if user input is
  if (data == "")
  {
    digitalWrite(led, HIGH);
    tone (buzzer, 100);
    Serial.println("Light is Switched ON");
    Serial.println("Buzzer is ON");
  }
  else if (data == "")
  {
    digitalWrite(led, LOW);
    noTone (buzzer);
    Serial.println("Light is Switched OFF");
    Serial.println("Buzzer is off");
  }
  data = "";
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
{
  char datestring[20];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
             dt.Day(),
             dt.Month(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second() );
  Serial.print(datestring);
}
