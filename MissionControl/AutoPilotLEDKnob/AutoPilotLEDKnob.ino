#include "LedControl.h" //  need the library
LedControl lc=LedControl(12,11,10,1); // lc is our object
// pin 12 is connected to the MAX7219 pin 1
// pin 11 is connected to the CLK pin 13
// pin 10 is connected to LOAD pin 12
// 1 as we are only using 1 MAX7219

#define PIN_ENCODER_A 3
#define PIN_ENCODER_B 2
#define MAX_LED_VALUE 180
volatile int lastEncoded = 0;
volatile long encoderValue = 0;
long lastEncoderValue = 0;
int lastMSB = 0;
int lastLSB = 0;
int encChange = 0;
int dir = 0;
int outputValue = 0;

void setup()
{
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0,false);// turn off power saving, enables display
  lc.setIntensity(0,8);// sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen
  Serial.begin(9600);

   pinMode(PIN_ENCODER_A, INPUT_PULLUP);
  pinMode(PIN_ENCODER_B, INPUT_PULLUP);

  attachInterrupt(0, UpdateEncoder, CHANGE);
  attachInterrupt(1, UpdateEncoder, CHANGE);
  WriteTo4DigitLED(0, 0);
}
/* 
 * Display a (hexadecimal) digit on a 7-Segment Display
 * Params:
 * addr  address of the display
 * digit the position of the digit on the display (0..7)
 * value the value to be displayed. (0x00..0x0F)
 * dp    sets the decimal point.
 
void setDigit(int addr, int digit, byte value, boolean dp);
*/
int disp = 0;
void loop()
{
  dir = (encoderValue - encChange);
  if (dir > 0)
    dir = 1;
  else if (dir < 0)
    dir = -1;

  switch (dir)
  {
    case 1:
      if (outputValue < MAX_LED_VALUE)
      {
        outputValue += 10;
        outputValue = min(MAX_LED_VALUE, outputValue);
        Serial.println(outputValue);
        WriteTo4DigitLED(0, outputValue);
      }
      break;
    case -1:
      if (outputValue > (-1*MAX_LED_VALUE))
      {
        outputValue -= 10;
        outputValue = max((-1*MAX_LED_VALUE), outputValue);
        Serial.println(outputValue);
        WriteTo4DigitLED(0, outputValue);
      }
      break;
  }
  encChange = encoderValue;

  WriteTo4DigitLED(0, outputValue);

  
  encChange = encoderValue;
  delay(40);
  /*
  for (int a=0; a<8; a++)
  {
    lc.setDigit(0,a,a,true);
    delay(100);
  }
  for (int a=0; a<8; a++)
  {
    lc.setDigit(0,a,8,1);
    delay(100);
  }
  for (int a=0; a<8; a++)
  {
    lc.setDigit(0,a,0,false);
    delay(100);
  }
  for (int a=0; a<8; a++)
  {
    lc.setChar(0,a,' ',false);
    delay(100);
  }
  for (int a=0; a<8; a++)
  {
    lc.setChar(0,a,'-',false);
    delay(100);
  }
  for (int a=0; a<8; a++)
  {
    lc.setChar(0,a,' ',false);
    delay(100);
  }
  */
}

void UpdateEncoder()
{
  int MSB = digitalRead(PIN_ENCODER_A);
  int LSB = digitalRead(PIN_ENCODER_B);

  int encoded = (MSB<<1)|LSB;
  int sum = (lastEncoded << 2) | encoded;

  switch (sum)
  {
    case 0b1101:
    case 0b0100:
    case 0b0010:
    case 0b1011:
      encoderValue++;
      break;
    case 0b1110:
    case 0b0111:
    case 0b0001:
    case 0b1000:
      encoderValue--;
      break;
    default:
      break;
  }
  lastEncoded = encoded;
}

void WriteTo4DigitLED(int index, int value)
{
  int ones;
  int tens;
  int hundreds;

  int neg = value < 0;
  int v = abs(value);
  if(v > 999) 
     return;

  ones=v%10;
  v=v/10;
  tens=v%10;
  v=v/10;
  hundreds=v;     
  if(neg) {
     //print character '-' in the leftmost column 
     lc.setChar(index,3,'-',false);
  }
  else {
     //print a blank in the sign column
     lc.setChar(index,3,' ',false);
  }
  //Now print the number digit by digit
  lc.setDigit(index,2,(byte)hundreds,false);
  lc.setDigit(index,1,(byte)tens,false);
  lc.setDigit(index,0,(byte)ones,false);
}

