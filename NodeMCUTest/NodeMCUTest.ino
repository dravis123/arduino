#include <ESP8266WiFi.h>


#define THROTTLE_PIN A0


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); // use the serial port
  Serial.println();Serial.println();
  Serial.println("Begin reading");
}

void loop() {
  // put your main code here, to run repeatedly:
  int throttleRaw = analogRead(THROTTLE_PIN);
  Serial.println(throttleRaw);

}

