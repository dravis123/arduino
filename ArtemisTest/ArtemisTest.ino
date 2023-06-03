
int led = 0;

void setup()
{
   Serial.begin(115200);
   Serial.println("Setup");

}

void loop()
{
    led = !led;
    digitalWrite(13, led);
    delay(1000);

}