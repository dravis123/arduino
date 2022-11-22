//#include <BlockDriver.h>
#include <FreeStack.h>
#include <MinimumSerial.h>
#include <SdFat.h>
#include <SdFatConfig.h>
#include <FastLED.h>
#include <Wire.h>
#include <SPI.h>



#define LED_DATA_PIN 3
#define SD_CHIP_SELECT_PIN 53
// Pins 13/12/11/10 reserved for SD card

#define SD_MIN_READ_INTERVAL 2000 // prevent us from reading SD card more than every 2 secs
SdFat SD;

unsigned long nextReadTime;

#define NUM_LEDS 42
#define NUM_NOTES 8
#define NUM_ROW_SPACER_LEDS 5 // 5 leds per row between bells
#define LOCAL_BUFFER_SONG_LENGTH 3000
#define RELAY_ON_TIME 31 // millisecs to actuate relays
#define RELAY_GREEN_BELL 29

#define BELL_PORT_LOWC 5
#define BELL_PORT_D 6
#define BELL_PORT_E 7
#define BELL_PORT_F 8
#define BELL_PORT_G 9
#define BELL_PORT_A 10
#define BELL_PORT_B 11
#define BELL_PORT_C 12


#define NUM_SONGS 11 // MAKE SURE NUM SONGS MATCHES BELOW
#define LAST_SONG (NUM_SONGS-1)
#define START_SONG 0 //for debugging
// Great reference for PROGMEM: https://www.e-tinkers.com/2020/05/do-you-know-arduino-progmem-demystified/
// save some chars
const char angels_on_high[] PROGMEM = {"00050000000500000005000000140000001400000000000A0005000000000000000400000002000000040000001000000004000000000002000100000000000000050000000500000005000000140000001400000000000A0005000000000000000400000002000000040000001000000004000000000002000100000000000000140000000000000020001000080004000A00000000000000100008000400020005000000000000000800040002000100020000000000100010000000000000000100000002000000040000000800000004000000000000000200000000000000140000000000000020001000080004000A000000000000001000080004000200050000000000000008000400020001000200000000001000100000000000000001000000020000000400000008000000040000000000000002000000000000000100000000000000000000"};
const char away_in_a_manger[] PROGMEM = {"00140000001400000000000A00040000000400000002000000800000008000000040000000200000001000000000000000100000001000000000002000100000001000000002000000400000002000000010000000010000000400000000000000140000001400000000000A0005000000050000000200000080000000800000004000000020000000100000000000000010000000080000000000040002000000040000000200000001000000020000002000000040000000800000000000000000000000000000"};
const char deck_the_halls[] PROGMEM = {"0010000000000008000400000002000000010000000200000004000000010000000200040008000200040000000000020001000000400000008000000000000000100000000000080004000000020000000100000002000000040000000100000002000400080002000400000000000200010000004000000080000000000000000200000000000400080000000200000004000000000008001000000002000000040008001000000020004000800000004000000020000000100000000000000010000000000008000400000002000000010000000200000004000000010000002000200020002000100000000000080004000000020000000100000000000000000000000000000000000000000000"};
const char do_you_hear[] PROGMEM = {"00010002000400000004000000040004000200020004000000000000000000000000000000010001001000100080000000100000000000000000000000000000000000000004000400040004000400000002000200040000000000000000000000000000000100010010001000800000001000000000000000000000000000000000001000250000000000040005000000000000001000200010000800040000000400020001000000010000000200000001000200040000000000000000000000040002000100000001000000080000000400020001000000000000000000000000000000000000000000000000000000000000"};
const char good_king[] PROGMEM = {"000100000001000000010000000200000001000000010000001000000000000000280000001400000028000000500000008400000000000000840000000000000001000000010000000100000002000000010000000100000010000000000000002800000014000000280000005000000084000000000000008400000000000000100000000800000004000000020000000400000002000000010000000000000028000000140000002800000050000000840000000000000084000000000000001000000010000000200000004000000001000000010000000200000000000000100000000800000004000000020000000100000000000000080000000000000001000000000000000000000000000000000000000000000000000000000000"};
const char hark_the_herald[] PROGMEM = {"00100000008000000080000000000040008000000004000000040000000200000014000000140000001400000000000A00050000000200000005000000000000001000000080000000800000000000400080000000040000000400000002000000100000000200000002000000000080004200000022000000100000000000000014000000140000001400000001000000080000000400000004000000020000001400000014000000140000000100000008000000040000000400000002000000280000000000280028000000120000000A000000050000000A000000000000000200000004000800100000000000010001000000020000000400000000000000280000000000280028000000120000000A000000050000000A00000000000000020000000400080010000000000001000100000002000000010000000000000000000000000000"};
const char jingle_bells[] PROGMEM = {"0010000000040000000200000001000000140000000000000000000000000010001000000004000000020000000100000028000000000000000000000000000000200000000800000004000000020000005000000000000000000000000000000010000000100000000800000002000000050000000000000000000000000000001000000004000000020000000100000014000000000000000000000010001000100000000400000002000000010000002000000000000000000000000000000028000000280000000A00000005000000020000001400000014000000140000001400000008000000140000000A0000000200000001000000000000001000000000000000050000000500000005000000000000000500000005000000050000000000000005000000140000000100000000000200050000000000000000000000000000000A0000000A0000000A00000000000A000A0000000500000005000000050005000400000002000000020000000500000002000000000000001400000000000000050000000500000005000000000000000500000005000000050000000000000005000000140000000100000000000200050000000000000000000000000000000A0000000A0000000A00000000000A000A00000005000000050000000500050014000000140000000A000000020000000100000000000000950000000000000000000000000000"};
const char joy_to_the_world[] PROGMEM = {"0080000000000000004000000000002000100000000000000000000000080000000400000000000000020000000000000001000000000000000000000010000000280000000000000000000000280000005000000000000000000000005000000084000000000000000000000000000000000000000000000000000000800000008000000040000000200000001000000010000000000008000400000080000000800000004000000020000000100000001000000000000800040000000400000004000000040000000400000004000800100000000000000000000000080004000200000002000000020000000200040008000000000000000000000004000200010000008000000000000000280000001400000000000A00050000000800000005000000000000000200000000000000010000000000000000000000000000"};
const char linus_lucy[] PROGMEM = {"0001000000100080000100100080000000010000001000200001001000200000000100000010008000010010008000000001000000100020000100100020000000000000000000000001000200040000000400020001000000020000000000010000000000000000000100020004000000040000000000000000000000000000000000000000000000010002000400000004000200010000000200000000000100000000000000000001000000000002000200000000000000000000000000000000000000000000000100000000000200020000000000000000000000000000000000000000000000010002000400000004000200010000000200000000000100000000000000000001000200040000000400000000000000000000000000000000000000000000000100020004000000040002000100000002000000000001000000000000000000010000000000020002000000000000000000000000000000000000000000000001000000000002000200000000000000000000000000000000000000000000"};
const char silent_night[] PROGMEM = {"001400000000002800140000000500000000000000000000001400000000002800140000000500000000000000000000000200000000000000020000004200000000000000000000008000000000000000800000001000000000000000000000002800000000000000280000008000000000005000280000001400000000002800100000000500000000000000000000002800000000000000280000008000000000005000280000001400000000002800100000000500000000000000000000000200000000000000020000000800000000000200400000000100000000000000000000000400000000000000000000008100000010000000040000001200000000000800020000000100000000000000000000000000000000"};
const char the_first_noel[] PROGMEM = {"000400020001000000000002000400080014000000000000002800500084000000500000002800000014000000000000002800480084000000400000002800000010000000280000004800000084000000140000000A00000005000000000000000400020001000000000002000400080012000000000000002800500084000000500000002800000014000000000000002800480080000000400000002800000010000000280000004200000084000000120000000A00000005000000000000000400020001000000000002000400080010000000000000008000400020000000000040000100020005000000000000000200000084000000400000002000000010000000200000004200000084000000120000000A00000005000000000000"};

PGM_P const songs_progmem[] PROGMEM =
{
    deck_the_halls,
    do_you_hear,
    jingle_bells,
    hark_the_herald,
    silent_night,
    away_in_a_manger,
    joy_to_the_world,
    linus_lucy,
    the_first_noel,
    angels_on_high,
    good_king
};

typedef struct{
  int bpm;
  int sideSwitchRate;
  String fileName;
  String data;
} T_SONG;

typedef struct{
  int msecs_dlay;
  int sideSwitchRate;
  int len;
  char fileName[32];
  char data[LOCAL_BUFFER_SONG_LENGTH];
} T_SONG_LOCAL;

T_SONG songs[NUM_SONGS]=\
{
  {320, 4, "deck.txt",     ""},
  {240, 4, "hear.txt",     ""},
  {320, 4, "jingle.txt",     ""},
  {280, 4, "herald.txt",  ""},
  {300, 6, "silent.txt",     ""},
  {240, 3, "manger.txt", ""},
  {300, 4, "joy.txt", ""},
  {300, 4, "linus.txt",     ""},
  {280, 4, "noel.txt",   ""},
  {300, 4, "angels.txt",   ""},
  {300, 4, "king.txt",        ""}
};

// Change this back to 0
int songNumGlob = START_SONG;
int id = 0;
int songBeatPosition = 0;
//int songReadFailCount = 0;
int getNewSong = 0;
int loadSong = 0;
#define MAX_SONG_MASK_NUM (1U<<NUM_NOTES)
T_SONG * pSong;
//PCF8574 pExp_relay(0x21);
// PORT EXPANDER HAS BUGS AND MAKES ALL BELLS CHIME
/*
microSD -> Arduino Uno
GND -> GND
VCC -> 5v
MISO -> 12
MOSI -> 11
SCK -> 13
CS -> 53
 */

/* 2-byte sound encoding:
                   noteB[0]               noteB[1]
Bell ID  15-14-13-12-11-10- 9- 8 | 7- 6- 5- 4- 3- 2- 1- 0
Note             UNUSED            C- B- A- G- F- E- D- c
DIO Out          UNUSED           12-11-10- 9- 8- 7- 6- 5
id indicates bell ID
*/

int noteLEDBegin[NUM_NOTES] = {1,6,11,16,37,32,27,22};
int noteLEDEnd[NUM_NOTES]   = {4,9,14,19,40,35,30,25};
int edgeLEDRow1[NUM_ROW_SPACER_LEDS] = {0,5,10,15,20};
int edgeLEDRow2[NUM_ROW_SPACER_LEDS] = {21,26,31,36,41};
uint32_t row1Color;
uint32_t row2Color;
T_SONG_LOCAL currSong;

typedef struct 
{
  int port;
  uint32_t color;
  String note;
} T_BELL;
//white yellow orange red purple blue cyan green
T_BELL bells[NUM_NOTES]=\
{
  {BELL_PORT_LOWC, CRGB::Red,    "c"},
  {BELL_PORT_D, CRGB::DarkOrange,"D"},
  {BELL_PORT_E, CRGB::Yellow,    "E"},
  {BELL_PORT_F, CRGB::Green,     "F"},
  {BELL_PORT_G, CRGB::Cyan,      "G"},
  {BELL_PORT_A, CRGB::Blue,      "A"},
  {BELL_PORT_B, CRGB::Purple,    "B"},
  {BELL_PORT_C, CRGB::GhostWhite,"C"}
};

CRGB leds[NUM_LEDS];

void setup() {
  for (int i = 0; i < NUM_NOTES; i++)
  {
    pinMode(bells[i].port, OUTPUT);  
  }
  SendAllOff();
  FastLED.addLeds<NEOPIXEL, LED_DATA_PIN>(leds, NUM_LEDS);
    fill_solid(leds, NUM_LEDS, CRGB::Red);
  row1Color = CRGB::Red;
  row2Color = CRGB::Green;
  FastLED.show();
  memset(&currSong,0,sizeof(currSong));
  Serial.begin(115200);
  //delay(1000);
  //InitSDCard();
  delay(1000);
  GetNextSong(songNumGlob);


}
int nextSongNum = 0;
void loop()
{
  char note[5];
  byte noteB[2];
  char message[128];
  // If song is over
  if ( (id+3 >= currSong.len) || (!currSong.len) )
  {
    Serial.println("  need new song");
    
    //nextSongNum = songNumGlob + 1;
    songNumGlob++;
    if (songNumGlob > LAST_SONG) // If beyond desired range, go to song 0
      songNumGlob = START_SONG;
    Serial.print("new song: ");Serial.println(nextSongNum);

    int return_code = 0;
    return_code = GetNextSong(songNumGlob);
    id = 0;
    delay(500);
  }

  int bellOnTime = RELAY_ON_TIME;
  int bellOffTime = currSong.msecs_dlay;
  //pSong = &songs[songNum];

  // Start by turning off all LEDs
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  memset(note,0,sizeof(note));
  memcpy(note, &currSong.data[id], 4);
  note[4] = '\0';
  uint16_t noteMask = (int)strtol(&note[0], NULL, 16);

  // Only used to print debug info to console
  memset(message,0,sizeof(message));
  sprintf(message, "char %s | note %d | id %d byte %x", note, noteMask, id, noteB[1]);
  
  Serial.println(message);
  //Serial.print(currSong.fileName);
  if (noteMask > 0)
  {
    PlayNote(noteMask);
    Serial.print("note: ");Serial.println(noteMask, BIN);
    
  }
  uint8_t b = (uint8_t)(noteMask&255);
 
  
  if ((songBeatPosition % currSong.sideSwitchRate) == 0) // Every N beats, change row edge colors
    SwitchEdgeRowColors();

  SetEdgeRowColors(); // Always update row edge colors
  FastLED.show();
  
  delay(bellOnTime); // Power on time for solenoids
  SendAllOff(); // Retract all solenoids
  
  id += 4; // Move up 4 chars (2 bytes)
  songBeatPosition++; // Track beat position
  delay(bellOffTime);

  
}
// Returns 0 on success, 1 on error
int GetNextSong(int songId)
{

  Serial.print("songId ");Serial.print(songId);Serial.println(" | ");
  unsigned long now = millis();
  if (songId > NUM_SONGS)
  {
    Serial.print("ERROR bad song id: ");Serial.println(songId);
    return 1;
  }
/*
  if (now < nextReadTime)
  {
    Serial.println("ERROR: cant read SD for 2 secs");
    Serial.print("now ");Serial.print(now);Serial.print(" nextRead: ");Serial.println(nextReadTime);
    return SD_ACCESS_TOO_FAST;
  }
  */
  // TODO: read these from SD card 
  //songs[id].data.toCharArray(currSong.data, sizeof(currSong.data));
  int rc = 0;
  //rc = LoadSongFromSDCard(songId);
  rc = LoadSongFromEEPROM(songId);
  if (rc)
  {
    Serial.print("Error reading song id ");Serial.print(songId);Serial.print(" code: ");Serial.println(rc);
    return rc;
  }

  // Check to make sure song is not corrupt
  Serial.print("songId ");Serial.print(songId);Serial.println(" | ");
  //nextReadTime = now + SD_MIN_READ_INTERVAL;
  songs[songId].fileName.toCharArray(currSong.fileName, sizeof(currSong.fileName));
  currSong.msecs_dlay = GetMsecsFromBPM(songs[songId].bpm) - RELAY_ON_TIME;
  currSong.sideSwitchRate = songs[songId].sideSwitchRate;
  currSong.len = strlen(currSong.data);
  Serial.print("Playing song: ");Serial.println(currSong.fileName);  
  Serial.print("bpm ");Serial.println(songs[songId].bpm);
  return 0;
}
/*
int CheckSongForCorruption()
{
  char note[5];
  for (int i = 0; i < sizeof(currSong.data); i+= 4)
  {
    memset(note,0,sizeof(note));
    memcpy(note, &currSong.data[i], 4);
    note[4] = '\0';
    uint16_t noteMask = (int)strtol(&note[0], NULL, 16);

    if (noteMask >= MAX_SONG_MASK_NUM)
    {
      Serial.print("Error, corrupt note ");Serial.print(noteMask);Serial.println(" found in song");
      return 1;
    }
  }
  Serial.println("Verified song not corrupt ");
  return 0;
}
*/

int LoadSongFromEEPROM(int songId)
{
  if (songId >= NUM_SONGS)
    return -1;

  
  memset(&currSong.data[0],0,sizeof(currSong.data));
  int len = strlen_P(songs_progmem[songId]);
  strcpy_P(currSong.data, (PGM_P)pgm_read_word(&songs_progmem[songId]));
  Serial.print("read song ");Serial.print(songId);Serial.print(" from progmem, len=");Serial.println(len);
  return 0;
}
/* SD card has no end of issues, bad reads, corruption. use EEPROM instead
int LoadSongFromSDCard(int songId)
{
  File file;
  memset(&currSong.data[0],0,sizeof(currSong.data));
  // Create the file.
  Serial.print("try to open file ID ");Serial.println(songId);Serial.print(" fname: ");Serial.println(songs[songId].fileName);
    file = SD.open(songs[songId].fileName, O_READ);
    if (!file) {
      file.close();
      Serial.println("open failed");
      return 1;
  }
  if (!file.available())
  {
      Serial.println("empty file");
  }

  // Now read file into the buffer
  if (file.read(&currSong.data[0], LOCAL_BUFFER_SONG_LENGTH) == 0)
  {
    // read error
    Serial.println("READ ERROR");
    file.close();
    return SONG_READ_FAIL_CODE;
  }

  
  file.close();
  Serial.print("Successfully read file ");Serial.println(songs[songId].fileName);
  Serial.println(currSong.data);

  return 0;
}
*/

void SendAllOff()
{
  for (int i = 0; i < NUM_NOTES; i++)
  {
    digitalWrite(bells[i].port, 0);  
  }
  
}

void SwitchEdgeRowColors()
{
  if (row1Color == CRGB::Red)
    row1Color = CRGB::Green;
  else
    row1Color = CRGB::Red;

  if (row2Color == CRGB::Red)
    row2Color = CRGB::Green;
  else
    row2Color = CRGB::Red;
}

void SetEdgeRowColors()
{
  for (int i = 0; i < NUM_ROW_SPACER_LEDS; i++)
  {
    leds[edgeLEDRow1[i]] = row1Color;
    leds[edgeLEDRow2[i]] = row2Color;
  }
}

int GetMsecsFromBPM(int BPM)
{
  // msecs = 1000 / ((BPM / 60))
  return (1000 / (BPM / 60));
}

void PlayNote(uint16_t bellMask)
{
  uint8_t bellLow = bellMask & 255; //
  //TODO: when we expand to 16 notes, look at high byte here
  // Remember, we relays are inverted (0=on 1=off) so write the NOT(mask)
  //pExp_relay.write8(bellLow);
  //Serial.print("port output: ");Serial.println(bellLow, BIN);
  for (int b = 0; b < 8; b++)
  {
    if (BitIsSet(&bellLow, b))
    {
      //Serial.print("Set bell ");Serial.print(b);
      //Serial.print(" note ");Serial.print(bells[b].note);
      //Serial.print(" port ");Serial.println(bells[b].port);
      
      digitalWrite(bells[b].port, 1);

      SetNoteColor(b, bells[b].color);
    }
  }
   
}
/*
void SendNote(int note)
{
  //int lowb = note&0xFF;
  //int highb = note&0xFF00;
  int port = 0;
  for (int b = 0; b < 8; b++)
  {
    port = b + OUTPUT_PIN_OFFSET;
    int chkBit = 1 << b;
    if (BitIsSet(b, &noteB)
    //if ( lowb & chkBit)
    
   {
      Serial.print("Set bell ");
      Serial.println(port);
      digitalWrite(port, 1);
    }
  }
}
*/
void SetNoteColor(int noteId, uint32_t color)
{
  if (noteId >= NUM_NOTES)
    return;
    
  for (int i = noteLEDBegin[noteId]; i <= noteLEDEnd[noteId]; i++)
  {
    if (i < NUM_LEDS)
      leds[i] = color;
  }
  //FastLED.show();
}

bool BitIsSet(uint8_t * mask, int b)
{
  return (*mask)&(1U<<b);
}
/*
void InitSDCard()
{
   if (!SD.begin(SD_CHIP_SELECT_PIN, SD_SCK_MHZ(50)))
   {
     SD.initErrorHalt();
     Serial.println("ERROR INIT SD CARD");
     return;
   }
   Serial.println("SD Card Init successful!");
}
*/
