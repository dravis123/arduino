#include <Servo.h>

#define PWM_OUTPUT_PIN 5
#define ANALOG_INPUT_PIN A0
#define DIRECTION_PIN 8 // Not used
#define MAX_SENSOR_INPUT_VALUE 1023

// I found the PWM range needs to be a tiny big larger to hit max forward throttle
#define PWM_MAX_FORWARD_THROTTLE 2025
#define PWM_MAX_REVERSE_THROTTLE 975
#define PWM_NEUTRAL_THROTTLE 1500

#define USE_MANUAL_INPUT 0

int motorSetting = 0;
int sensorValue = 0;
int manualInputValue = 0;
uint32_t prevSensorValue = 0;
int8_t dirValue = 1;
int dir = 1;
Servo talon;

void setup() {
  Serial.begin(9600);
  Serial.println("Setup");

  talon.attach(PWM_OUTPUT_PIN);
  pinMode(DIRECTION_PIN, INPUT_PULLUP);
}

bool newValue = false;
void loop() {
#if (USE_MANUAL_INPUT == 1)
  if (Serial.available() > 1)
  {
    manualInputValue = Serial.parseInt();
    newValue = true;
  }
#endif

  sensorValue = analogRead(ANALOG_INPUT_PIN);
  dirValue = digitalRead(DIRECTION_PIN);
  sensorValue = sensorValue - (sensorValue % 5);

  if (sensorValue != prevSensorValue)
  {
    newValue = true;
  }

  if (sensorValue < 0 || sensorValue > MAX_SENSOR_INPUT_VALUE)
    sensorValue = 0;

  // Only change direction if we are idle
  if (sensorValue == 0)
  {
    if (dirValue == 0) // forward
      dir = 1;
    else
      dir = -1;
      
  }

  motorSetting = 0;
#if (USE_MANUAL_INPUT == 0)
  motorSetting = map(sensorValue*dir, -1*MAX_SENSOR_INPUT_VALUE, MAX_SENSOR_INPUT_VALUE, PWM_MAX_REVERSE_THROTTLE, PWM_MAX_FORWARD_THROTTLE);
#else
  motorSetting = map(manualInputValue, -1*MAX_SENSOR_INPUT_VALUE, MAX_SENSOR_INPUT_VALUE, PWM_MAX_REVERSE_THROTTLE, PWM_MAX_FORWARD_THROTTLE);
#endif

  talon.writeMicroseconds(motorSetting);
  if (newValue)
  {
    Serial.print("in: ");
#if (USE_MANUAL_INPUT == 0)
    Serial.print(sensorValue);
#else
    Serial.print(manualInputValue);
#endif
    Serial.print(" out: ");
    Serial.println(motorSetting);
  }

  prevSensorValue = sensorValue;
  newValue = false;

}
