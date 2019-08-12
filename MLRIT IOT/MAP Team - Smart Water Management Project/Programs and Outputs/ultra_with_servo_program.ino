#include <Servo.h>
Servo servo1;
int trigPin = D1;
int echoPin = D2;
long distance;
long duration;
 
void setup() 
{
  servo1.attach(D3); 
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
}
 
void loop() 
{
  ultra();
  servo1.write(0);
  if(distance <= 10)
  {
    servo1.write(90);
  }
  else
  {
    servo1.write(0);
  }
}
 
void ultra()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration*0.034/2;
  Serial.print("object distance is : ");
  Serial.println(distance);
  delay(5000);
}
