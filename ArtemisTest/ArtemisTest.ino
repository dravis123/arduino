#include <Servo.h>

Servo talonL;
Servo talonR;
int led = 0;

void setup()
{
   Serial.begin(115200);
   Serial.println("Setup");
   talonL.attach(22);
   talonR.attach(4);

}

void loop()
{
    led = !led;
    //digitalWrite(13, led);
    talonL.writeMicroseconds(1500);
    talonR.writeMicroseconds(2000);
    //delay(100);

}