#include <FastLED.h>
#include "ChristmasTree.h"
#define NUM_LEDS 295
#define NUM_LEDS_WITH_STAR 300
#define NUM_LEDS_STAR 5
#define LED_PIN 7
#define DEFAULT_DELAY_INTERVAL 20
CRGB leds[NUM_LEDS_WITH_STAR];
byte ledHue[NUM_LEDS_WITH_STAR];

#define NUM_LED_ROWS 7
T_SHOW_TYPE shows[E_SHOW_MAX_NUM];

typedef struct
{
  byte xPos;
  byte yPos;
} T_LED;

//T_LED ledPosition[NUM_LEDS] ;

#define BASE_COLOR       CRGB(155,155,140)
#define PEAK_COLOR       CRGB(255,255,240)
#define DELTA_COLOR_UP   CRGB(10,10,10)
#define DELTA_COLOR_DOWN CRGB(5,5,5)


#define CHANCE_OF_TWINKLE 127
byte PixelState[NUM_LEDS_WITH_STAR];
uint8_t currShow;

//DEFINE_GRADIENT_PALETTE( twinkle_gp ) {
//  0,   127,127,127,  //white
//100,   250,250,250,  //red
//150,   250,250,250,  //red 
//200,   127,127,127}; //green
uint64_t showDefaultDuration = 60000;
int64_t showStartTime = 0;
CRGBPalette16 myPal = OceanColors_p ; //OceanColors_p HeatColors_p PartyColors_p LavaColors_p 
//CRGBPalette16 twinklePal = twinkle_gp;
void setup() {
  Serial.begin(115200); // use the serial port
  Serial.print("Create LED...");
  FastLED.setBrightness(90);
  //FastLED.setDither( 0 );
  FastLED.addLeds<WS2811, LED_PIN, RGB>(leds, NUM_LEDS_WITH_STAR).setCorrection( TypicalSMD5050 );
  InitPixelStates();
  InitShowDefaults();
  currShow = E_SHOW_CHASE;
  showStartTime = millis();
  Serial.println("Done");
  fill_solid(leds, NUM_LEDS, CRGB::White);
  //fill_solid(&leds[NUM_LEDS], 5, CRGB::Blue);
  RandomizeStartHues();
  // COLOR CORRECTIon http://fastled.io/docs/3.1/group___color_enums.html#details
  
}

int chaseId1 = 0;
int chaseId2 = 75;
int chaseId3 = 150;
int chaseId4 = 225;
int serialInput = 0;
byte hueId = 0;
byte hueChangeInterval = 1;
int delayInterval = DEFAULT_DELAY_INTERVAL;
uint32_t loopId = 0;
void loop() 
{
  //while (Serial.available() > 0) 
  //{
    
    // get incoming byte:
  //  serialInput = Serial.parseInt();
  //  if (Serial.read() == '\n')
  //    break;
  //}
  //if (hueId % 10 == 0)
  //{
    //Serial.print("Hue ID: ");Serial.println(hueId);
    //Serial.print("Test ID: ");Serial.println(serialInput);
  //}

  int64_t showDuration = millis() - showStartTime;
  if ( showDuration > showDefaultDuration)
  {
    NextShow();
  }
  
  RunLEDShow(currShow);
  TwinkleStar();
  FastLED.show();
  FastLED.delay(delayInterval);
  if (loopId % hueChangeInterval == 0)
    hueId++;

  loopId++;
  //hueId++;
}

void RunLEDShow(uint8_t show)
{
  switch(show)
  {
    case E_SHOW_TWINKLE_COLOR:
      Twinkle(hueId, 255);
      break;
    case E_SHOW_CHASE:
      ChaseShow();
      break;
    case E_SHOW_MULTICOLOR:
      Multicolored();
      break;
    case E_SHOW_DEFAULT:
    case E_SHOW_TWINKLE:
    default:
      Twinkle(182, 255); // Use dark blue instead of white
      break;
      //fill_solid(leds, NUM_LEDS, CRGB::White);
      //TwinkleMapPixels();
      //RainbowTest();
      //SteadyIncandescent();
      //fill_solid(leds, NUM_LEDS, CHSV(hueId, 255,255));
      //fill_solid(leds, NUM_LEDS,ColorFromPalette( myPal, hueId));
      //RandomColorPalette();
      //Twinkle();
      //Multicolored();
      //Chase(CRGB::Red, CRGB::Green);
      //fill_solid(leds, NUM_LEDS, CRGB::White);
      //TestLEDMap();
  }
}

void NextShow()
{
  int nextId = GetWrappedValue(currShow, 1, E_SHOW_MAX_NUM-1);
  currShow = nextId;
  hueChangeInterval = 1;
  showStartTime = millis();
  Serial.print("Starting show ");Serial.println(currShow);
  switch (currShow)
  {
    case E_SHOW_MULTICOLOR:
      RandomizeStartHues();
      break;
    case E_SHOW_CHASE:
      delayInterval = 10;
    default:
      delayInterval = DEFAULT_DELAY_INTERVAL;
  }
  
  
}

int GetWrappedValue(int a, int incr, int maxVal)
{
  int newVal = 0;
  if (incr < 0)
  {
    if (abs(incr) > a)
      newVal = incr+a+maxVal;
    else
      newVal = incr+a;
  }
  else
  {
    if (a+incr > maxVal)
    {
      newVal = a + incr - maxVal;
    }
    else
    {
      newVal = a+incr;
    }
    
  }
  return newVal;
  
}

void ChaseShow()
{
  //fill_solid(leds, NUM_LEDS, CHSV(0, 255, 255));
  Chase(&chaseId1, 0, 96);
  Chase(&chaseId2, 0, 0);
  Chase(&chaseId3, 0, 96);
  Chase(&chaseId4, 0, 0);
}

void Chase(int * chaseId, byte hue1, byte hue2)
{
  if (!chaseId)
    return;
    
  if (*chaseId >= NUM_LEDS)
    *chaseId = 0;

  leds[*chaseId] = CHSV(hue2, 255, 255);
/* Original
  int id[6];
  byte val[6] = {64,127,255,255,255,127};
  for (int i = 0; i < 6; i++)
  {
    int offset = i-4;
    id[i] = GetWrappedValue(*chaseId, offset, NUM_LEDS);
    leds[id[i]] = CHSV(hue2, 255, val[i]);
    //Serial.print("l[");Serial.print(i);Serial.print("]=");Serial.println(id[i]);
  }
  */
 
  (*chaseId)++;
  //Serial.println(chaseId);
}

void Multicolored()
{
  hueChangeInterval = 5;
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CHSV(++ledHue[i], 255, 255);
  }
}

void TestLEDMap()
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    if ((i+serialInput) % 20 == 0)
      leds[i] = CRGB::Blue;
    else
      leds[i] = CRGB::Black;
  }
}

void SteadyIncandescent()
{
  //FastLED.setTemperature( Tungsten40W );
  fill_solid(leds, NUM_LEDS, CRGB::White);
  
}
int hue = 0;
void RainbowTest()
{

  for (uint16_t i = 0; i< NUM_LEDS; i++)
  {
    leds[i] = CHSV(hueId+i, 255,255);
  }
  //CHSV color = CHSV( 224, 187, 255);

  //for (int r = 0; r < NUM_LED_ROWS; r++)
  //{
    //fill_solid( &(leds[i]), 1 /*number of leds*/, CHSV( 224, 187, 255) );
    //fill_solid( row[r], rowWidth[r] , CHSV( hue+(r*30), 255, 255) );
  //}
  //hue += 2;
}

void RandomizeStartHues()
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    ledHue[i] = random8();
  }
}

void RandomColorPalette()
{
  for( uint16_t i = 0; i < NUM_LEDS; i++) 
  {
    //leds[i] = ColorFromPalette( myPal, ++ledHue[i]);
  }
}

#define TWINKLE_PEAK 250
#define TWINKLE_BASE 150
#define TWINKLE_INC 5
#define TWINKLE_DEC 8
#define TWINKLE_PROBILITY 2

void InitPixelStates()
{
  memset( PixelState, sizeof(PixelState), 0); // initialize all the pixels to SteadyDim.
  fill_solid( leds, NUM_LEDS_WITH_STAR, CRGB(255, 0, TWINKLE_BASE));
}

void InitShowDefaults()
{
  memcpy(&shows,&defaultShow,sizeof(shows));
}
// hue=hueId, sat=255 for COLOR
// hue=255, sat = 0 for WHITE
void Twinkle(int hue, int sat)
{
  for( uint16_t i = 0; i < NUM_LEDS; i++) 
  {
    leds[i] = CHSV(hue,sat,TWINKLE_BASE);
    //if (i == 129)
    //{
    if (PixelState[i] == 0)
    {
      randomSeed(analogRead(0));
      ledHue[i] = TWINKLE_BASE;
      //leds[i] = CHSV(hueId,255,TWINKLE_BASE);
      if( random8() < TWINKLE_PROBILITY)
      {
        PixelState[i] = 1;
      }
    }
    
    if (PixelState[i] == 1)
    {
      leds[i] = CHSV(hue,sat,ledHue[i]);
      ledHue[i] += TWINKLE_INC;
      //leds[i] = ColorFromPalette( twinklePal, ledHue[i]);

      if (ledHue[i] > TWINKLE_PEAK)
      {
        PixelState[i] = 2;
      }
    }

    if (PixelState[i] == 2)
    {
      leds[i] = CHSV(hue,sat,ledHue[i]);
      ledHue[i] -= TWINKLE_DEC;
      //leds[i] = ColorFromPalette( twinklePal, ledHue[i]);

      if (ledHue[i] < TWINKLE_BASE)
      {
        PixelState[i] = 0;
      }
    }
  }
}

void TwinkleStar()
{
  for( uint16_t i = NUM_LEDS; i < NUM_LEDS_WITH_STAR; i++) 
  {
    if (i == 296)
    {
      //Serial.print("star ");Serial.print(i);Serial.print(" state ");Serial.print(PixelState[i]);Serial.print(" val ");Serial.println(ledHue[i]);
    }
    leds[i] = CHSV(255,0,TWINKLE_BASE);
    if (PixelState[i] == 0)
    {
      randomSeed(analogRead(0));
      ledHue[i] = TWINKLE_BASE;
      if( random8() < 5)
      {
        PixelState[i] = 1;
      }
    }
    
    if (PixelState[i] == 1)
    {
      leds[i] = CHSV(255,0,ledHue[i]);
      ledHue[i] += TWINKLE_INC;
      if (ledHue[i] > TWINKLE_PEAK)
      {
        PixelState[i] = 2;
      }
    }

    if (PixelState[i] == 2)
    {
      leds[i] = CHSV(255,0,ledHue[i]);
      ledHue[i] -= TWINKLE_DEC;
      if (ledHue[i] < TWINKLE_BASE)
      {
        PixelState[i] = 0;
      }
    }
    
  }
}
 
