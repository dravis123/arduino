#ifndef _CHRISTMASTREE_H
#define _CHRISTMASTREE_H

//https://www.instructables.com/id/Arduino-Controlled-Positional-RGB-LED-Christmas-Tr/

typedef struct
{
  CRGB color1;
  CRGB color2;
  uint32_t hsvColor1;
  uint32_t hsvColor2;
  uint8_t speed;
  uint8_t interval;
  uint8_t top;
  uint8_t left;
  uint8_t param[4];
} T_SHOW_TYPE;

static T_SHOW_TYPE defaultShow[] =\
{
  {CRGB::White, CRGB::Black, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}}, // Default
  {CRGB::White, CRGB::Black, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}}, // Twinkle
  {CRGB::White, CRGB::Black, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}}, // Twinkle color changing
  {CRGB::White, CRGB::Black, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}}, // chase
  {CRGB::White, CRGB::Black, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}}, // multicolor
  {CRGB::White, CRGB::Black, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}}, // dissolve
  {CRGB::White, CRGB::Black, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}}, // meld
  {CRGB::White, CRGB::Black, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}} // wave
};

typedef enum
{
  E_SHOW_DEFAULT = 0,
  E_SHOW_TWINKLE = 1,
  E_SHOW_CHASE   = 2,
  E_SHOW_TWINKLE_COLOR = 3,
  E_SHOW_MULTICOLOR  = 4, // horizontal or vertical
  //E_SHOW_COLOR_DISSOLVE = 4, //pixel by pixel changes to other color
  //E_SHOW_COLOR_MELD = 5, //colors change horizontall or vertically by rainbow band
  //E_SHOW_COLOR_WAVE = 6,
  E_SHOW_MAX_NUM
} E_LIGHT_SHOW;

#endif // _CHRISTMASTREE_H
