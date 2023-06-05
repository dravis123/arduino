#include <Servo.h>
#include "UTVECU.h"

int motorSetting = 0;
int throttleValue = 0;
int throttleReverse = 0;
int manualInputValue = 0;
int throttleAnalogIn = 0;
int throttleRawInput = 0;
uint32_t prevSensorValue = 0;
int8_t dirValue = 1;
int dir = 1;
int keyState = 0;
int keyValue = 0;
Servo talonL;
Servo talonR; // Mapped to two talons, split PWM cable

void setup()
{
   Serial.begin(115200);
   Serial.println("Setup");

   pinMode(DIRECTION_PIN, INPUT_PULLUP);
   pinMode(KEY_PIN, INPUT_PULLUP);
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
   throttleAnalogIn = analogRead(ANALOG_INPUT_THROTTLE);
   // Remove deadband in beginning of throttle values (scale 300-1024 to 0-1024 for easier logic later)
   throttleRawInput = map(throttleAnalogIn, MIN_THROTTLE_INPUT_VALUE, MAX_THROTTLE_INPUT_VALUE, 0, MAX_FORWARD_THROTTLE);
   throttleValue = GetThrottleDeadband(throttleRawInput);

   // Only change direction if we are idle
   if (throttleValue == 0)
   {
      dir = digitalRead(DIRECTION_PIN);
   }
   
   if (throttleValue != prevSensorValue)
   {
      newValue = true;
   }

   motorSetting = 0;
#if (USE_MANUAL_INPUT == 0)
   if (dir == DIRECTION_FORWARD_VALUE)
      motorSetting = map(throttleValue, 0, MAX_FORWARD_THROTTLE, PWM_NEUTRAL_THROTTLE, PWM_MAX_FORWARD_THROTTLE);
   else //FIX
      motorSetting = map(throttleValue, MAX_FORWARD_THROTTLE, 0, PWM_MAX_REV_THROTTLE, PWM_NEUTRAL_THROTTLE);
#else
   motorSetting = map(manualInputValue, -1 * MAX_SENSOR_INPUT_VALUE, MAX_SENSOR_INPUT_VALUE, PWM_MAX_REVERSE_THROTTLE, PWM_MAX_FORWARD_THROTTLE);
#endif

   // Check key state
   keyValue = !digitalRead(KEY_PIN); // invert the pin so 1 = ON
   if (keyState != keyValue)
   {
      Serial.print("KEY CHANGE: ");Serial.print(keyState);Serial.print(" to ");Serial.println(keyValue);
      keyState = keyValue;
      SetServoAttach(keyState);
   }

#if (THROTTLE_ENABLE_OUTPUT == 1)
   talonL.writeMicroseconds(motorSetting);
   talonR.writeMicroseconds(motorSetting);
#endif

   if (newValue)
   {
      if (keyState == 1)
         Serial.print("ON |");
      else
         Serial.print("OFF|");
      
      if (dir == DIRECTION_FORWARD_VALUE)
         Serial.print("FWD|");
      else
         Serial.print("REV|");
      Serial.print(" anlg: ");Serial.print(throttleAnalogIn);
      Serial.print(" raw: ");Serial.print(throttleRawInput);
      Serial.print(" tht: ");
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

static void SetServoAttach(int attach)
{
   if (attach == 1)
   {
      if (!talonL.attached(PWM_OUTPUT_MOTOR_L_PIN))
         talonL.attach(PWM_OUTPUT_MOTOR_L_PIN);//, PWM_MAX_REVERSE_THROTTLE, PWM_MAX_FORWARD_THROTTLE, PWM_NEUTRAL_THROTTLE);
      if (!talonR.attached(PWM_OUTPUT_MOTOR_R_PIN))
         talonR.attach(PWM_OUTPUT_MOTOR_R_PIN);//, PWM_MAX_REVERSE_THROTTLE, PWM_MAX_FORWARD_THROTTLE, PWM_NEUTRAL_THROTTLE);
         Serial.println("Servos ATTACHED");
   }
   else
   {
      if (talonL.attached(PWM_OUTPUT_MOTOR_L_PIN))
         talonL.detach();
      if (talonR.attached(PWM_OUTPUT_MOTOR_R_PIN))
         talonR.detach();
         Serial.println("Servos DETACHED");
   }
}

