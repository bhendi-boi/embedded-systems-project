#include <Arduino.h>

void setup()
{
  // put your setup code here, to run once:
  pinMode(DD7, OUTPUT);
}

void loop()
{
  // put your main code here, to run repeatedly:
  digitalWrite(DD7, HIGH);
  delay(1000);
  digitalWrite(DD7, LOW);
  delay(1000);
}