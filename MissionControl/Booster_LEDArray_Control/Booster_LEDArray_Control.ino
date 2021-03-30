#include "LedControl.h" //  need the library

#define MAX7291_PIN01 12
#define MAX7291_PIN13 11
#define MAX7291_PIN12 10
#define NUM_BAR_LEDS 20
#define MAX_FUEL_LEVEL 200
#define FUEL_LOW_USE -4
#define FUEL_MED_USE -8
#define FUEL_HI_USE -16

/*******PINS***********/
#define PIN_ENCODER_A 3
#define PIN_ENCODER_B 2
#define FUEL_CROSSFEED 4
#define BOOSTER_LOW 5
#define BOOSTER_MED 6
#define BOOSTER_HI 7
/**********************/
typedef enum
{
  CELL_A = 0,
  CELL_B = 1,
  E_NUM_FUEL_CELLS
  
} E_FUEL_CELL;
int fuel[E_NUM_FUEL_CELLS];


LedControl lc=LedControl(MAX7291_PIN01,
                         MAX7291_PIN13,
                         MAX7291_PIN12,
                         1);
void setup()
{
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0,false);// turn off power saving, enables display
  lc.setIntensity(0,8);// sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen
  pinMode(BOOSTER_LOW, INPUT_PULLUP);
  pinMode(BOOSTER_MED, INPUT_PULLUP);
  pinMode(BOOSTER_HI, INPUT_PULLUP);
  pinMode(FUEL_CROSSFEED, INPUT_PULLUP);
  fuel[CELL_A] = MAX_FUEL_LEVEL;
  fuel[CELL_B] = MAX_FUEL_LEVEL;
  Serial.begin(9600);
}
int leds = 0;

void loop()
{

  Serial.print("Booster Levels: ");
  Serial.print(fuel[CELL_A]);
  Serial.print(" ");
  Serial.println(fuel[CELL_B]);
  UpdateFuelCells();
  SetLEDBar(CELL_A, (fuel[CELL_A] * NUM_BAR_LEDS) / MAX_FUEL_LEVEL);
  SetLEDBar(CELL_B, (fuel[CELL_B] * NUM_BAR_LEDS) / MAX_FUEL_LEVEL);
  delay(1000);
}

void SetLEDBar(E_FUEL_CELL i, int num_leds)
{
  if (num_leds > NUM_BAR_LEDS)
    return;
 // bar 1 gets rows 0-2 and bar 2 gets 3-5
  int start_row = 0;
  if (i == CELL_B)
    start_row = 3;

  Serial.print("turn on ");
  Serial.print(num_leds);
  Serial.print(" :");
  int currLED = 0;
  for (int r = start_row; r < start_row+3; r++)
  {
    for (int c = 0; c < 8; c++)
    {
      if ( currLED < num_leds)
      {
        lc.setLed(0,r,c,true); // turns on LED at col, row
        Serial.print(1);
        //Serial.print("("); Serial.print(c); Serial.print(","); Serial.print(r); Serial.println(")");
      }
      else
      {
        lc.setLed(0,r,c,false); // turns on LED at col, row
        Serial.print(0);
      }
      currLED++;
      if (currLED >= NUM_BAR_LEDS)
      {
        Serial.println();
        return;
      }
    }
  }
  return;
}

void UpdateFuelCells()
{
  int crossfeed = !digitalRead(FUEL_CROSSFEED);
  int rate = GetFuelRateUsed();

  if (!crossfeed)
  {
    if (fuel[CELL_A])
    {
      AdjustTankValue(CELL_A, rate);
    }
    else
    {
      AdjustTankValue(CELL_B, rate);
    }
  }
  else
  {
    AdjustTankValue(CELL_A, rate/2);
    AdjustTankValue(CELL_B, rate/2);
  }
  
}

void AdjustTankValue(int cell, int amount)
{
  int newAmt = fuel[cell] + amount;
  
  if (newAmt > 0)
    fuel[cell] = newAmt;
  else
    fuel[cell] = 0;
}
int GetFuelRateUsed()
{
  
  if (!digitalRead(BOOSTER_HI))
    return FUEL_HI_USE;
  if (!digitalRead(BOOSTER_MED))
    return FUEL_MED_USE;
  if (!digitalRead(BOOSTER_LOW))
    return FUEL_LOW_USE;
    
  return 0;
}
/*
for (int row=0; row<8; row++)
  {
    for (int col=0; col<8; col++)
    {
      lc.setLed(0,col,row,true); // turns on LED at col, row
      delay(25);
    }
  }
  */



