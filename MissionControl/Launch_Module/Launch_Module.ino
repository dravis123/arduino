#define LAUNCH_ARM_SWITCH 10
#define LAUNCH_ENGAGE_BUTTON 11
#define LAUNCH_COUNTDOWN_TIMER_MS 10000

unsigned long launchTimer = 0;
unsigned long missionStartTime = 0;
int hasLaunched = 0;
int timerState;

typedef enum
{
  E_TIMER_NEUTRAL = 0,
  E_TIMER_COUNTDOWN = 1,
  E_TIMER_COUNTUP = 2,
  E_TIMER_MAX
} E_MISSION_TIMER_STATE


void setup() {
  // put your setup code here, to run once:
  pinMode(LAUNCH_ARM_SWITCH, INPUT_PULLUP);
  pinMode(LAUNCH_ENGAGE_BUTTON, INPUT_PULLUP);
  timerState = E_TIMER_NEUTRAL;
}

void loop() {
  // put your main code here, to run repeatedly:

  CheckLaunchState();
  UpdateLaunchClock();
}

void CheckLaunchState()
{
  if (hasLaunched)
    return;
    
  int isArmed = digitalRead(LAUNCH_ARM_SWITCH);
  int launchPressed = digitalRead(LAUNCH_ENGAGE_BUTTON);

  if (!isArmed)
    return;

  if (launchPressed)
  {
    missionStartTime = millis();
    hasLaunched = 1;
    timerState = E_TIMER_COUNTDOWN;
  }  
}

void UpdateLaunchClock()
{
  unsigned long timeNow = millis();
  unsigned long elapsed = launchTimer - timeNow;
  int timerSign = 0;
  switch (timerState)
  {
    case E_TIMER_NEUTRAL:
      ShowNeutralTimer();
      return;
    case E_TIMER_COUNTDOWN:
    {
      if (elapsed > LAUNCH_COUNTDOWN_TIMER_MS)
      {
        timerState = E_TIMER_LAUNCHED;
        launchTimer = timeNow;
        timerSign = 1;
      }
      else
         timerSign = -1;     
    }
    case E_TIMER_LAUNCHED:
      break;
  }
  
  SetLaunchClockLEDs(launchTimer % 1000, launchTimer/1000, timerSign);
}

void SetLaunchClockLEDs(int msecs, unsigned long secs, int sign)
{
  // TODO: set the timer leds here
}

