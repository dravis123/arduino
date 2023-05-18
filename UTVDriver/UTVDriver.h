
#define PWM_OUTPUT_MOTOR_L_PIN 5
#define PWM_OUTPUT_MOTOR_R_PIN 6
#define ANALOG_INPUT_THROTTLE A0
#define DIRECTION_PIN 8 // Not used
#define DIRECTION_FORWARD_VALUE 0
#define MAX_THROTTLE_INPUT_VALUE 1023

#define MAX_FORWARD_THROTTLE 1023
#define MAX_REVERSE_THROTTLE 350

#define THROTTLE_STEPS 3 // throttle input deadband
#define THROTTLE_ZERO_DEADBAND 30 //throttle input deadband +/-

// I found the PWM range needs to be a tiny big larger to hit max forward throttle
#define PWM_MAX_FORWARD_THROTTLE 2025
#define PWM_MAX_REVERSE_THROTTLE 975
#define PWM_NEUTRAL_THROTTLE 1500

#define USE_MANUAL_INPUT 0
#define THROTTE_USE_SQUARED_INPUTS 1