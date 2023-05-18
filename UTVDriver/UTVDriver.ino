#include <Servo.h>
#include "UTVDriver.h"

int motorSetting = 0;
int throttleValue = 0;
int manualInputValue = 0;
int throttleRawInput = 0;
uint32_t prevSensorValue = 0;
int8_t dirValue = 1;
int dir = 1;
Servo talonL;
Servo talonR; // Mapped to two talons, split PWM cable

void setup()
{
   Serial.begin(9600);
   Serial.println("Setup");

   talonL.attach(PWM_OUTPUT_MOTOR_L_PIN);
   talonR.attach(PWM_OUTPUT_MOTOR_R_PIN);
   pinMode(DIRECTION_PIN, INPUT_PULLUP);
}

bool newValue = false;
void loop()
{
#if (USE_MANUAL_INPUT == 1)
   if (Serial.available() > 1)
   {
      manualInputValue = Serial.parseInt();
      newValue = true;
   }
#endif
   throttleRawInput = analogRead(ANALOG_INPUT_THROTTLE);
   dirValue = digitalRead(DIRECTION_PIN);

   if (dir == -1)
   {
      throttleRawInput = max(throttleRawInput, MAX_REVERSE_THROTTLE);
   }

   throttleValue = GetThrottleDeadband(throttleRawInput);
   
   if (throttleValue != prevSensorValue)
   {
      newValue = true;
   }

   // Only change direction if we are idle
   if (throttleValue == 0)
   {
      if (dirValue == DIRECTION_FORWARD_VALUE) // forward
         dir = 1;
      else
         dir = -1;
   }



   motorSetting = 0;
#if (USE_MANUAL_INPUT == 0)
   motorSetting = map(throttleValue * dir, -1 * MAX_THROTTLE_INPUT_VALUE, MAX_THROTTLE_INPUT_VALUE, PWM_MAX_REVERSE_THROTTLE, PWM_MAX_FORWARD_THROTTLE);
#else
   motorSetting = map(manualInputValue, -1 * MAX_SENSOR_INPUT_VALUE, MAX_SENSOR_INPUT_VALUE, PWM_MAX_REVERSE_THROTTLE, PWM_MAX_FORWARD_THROTTLE);
#endif

   talonL.writeMicroseconds(motorSetting);
   talonR.writeMicroseconds(motorSetting);

   if (newValue)
   {
      Serial.print("in: ");
#if (USE_MANUAL_INPUT == 0)
      Serial.print(throttleValue);
#else
      Serial.print(manualInputValue);
#endif
      Serial.print(" out: ");
      Serial.println(motorSetting);
   }

   prevSensorValue = throttleValue;
   newValue = false;
}

static int GetThrottleDeadband(int input)
{
   if (input > THROTTLE_ZERO_DEADBAND)
      return (input - (input % THROTTLE_STEPS));
   else 
      return 0;
}

