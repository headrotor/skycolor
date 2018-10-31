

/* skycolor (c) Jon Foote 2018
    Driver for Stochastic Labs light work
*/


/* pinout: Teensy 3.2
    Pin 1
    Pin 2 Ch A wheel encoder
    Pin 3 Ch B wheel encoder (US digital)
    Pin 4
    Pin 5
    Pin 6 WS812B neopixel data out
    Pin 7
    Pin 8
    Pin 9
    Pin 10 OLED SPI CS
    Pin 11 OLED SPI MOSI
    Pin 12
    Pin 13 OLED SPI CLK
    Pin 14
    Pin 15 OLED SPI DC
    Pin 16
    Pin 17
    Pin 18
    Pin 19
    Pin 20
    Pin 21
    Pin 22
    Pin 23

*/

#include <Arduino.h>

//#include "colors.h"
//const PROGMEM prog_uint32_t ctable[]={0x0}
#define CTAB_LEN 60
#define SECS_PER_C 1.0
//const PROGMEM prog_uint32_t mtable[CTAB_LEN];
uint32_t mtable[CTAB_LEN];




#include <FastLED.h>
#include <Encoder.h>
#include <SPI.h>
#define SLOW_SPI
#include <ssd1351.h>


/***********************************************FASTLED***********************************/




FASTLED_USING_NAMESPACE

#define DATA_PIN    5
//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
//#define NUM_LEDS    (32+24+16+12 +8 + 1 + 10)
#define NUM_LEDS    60

CRGB leds[NUM_LEDS];

#define BRIGHTNESS          255
#define FRAMES_PER_SECOND  120



/****************************************** ENCODER ********************/




// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder wheel(2, 3);

/****************************************** OLED ********************/



// think this is teensy only!
/* works with 128x128 OLED from here:

  https://www.amazon.com/gp/product/B01HHPOD44/
  "DIYmall 1.5 inch Colorful OLED SSD1351 128X128 Resolution"

  Pinout:
  GND
  VCC  3.3V from Teensy
  SCL  13
  SDA  11
  RES  nc
  DC   15
  CS   10
*/

//this to do Color c = RGB(...) instead of `RGB c = RGB(...)` or ssd1351::LowColor c = RGB(...)
// because it's slightly faster and guarantees you won't be sending wrong colours to the display.

// Choose color depth - IndexedColor, LowColor and HighColor currently supported
// typedef ssd1351::IndexedColor Color;
// typedef ssd1351::LowColor Color;
typedef ssd1351::HighColor Color;

// Choose display buffering - NoBuffer or SingleBuffer currently supported
// auto display = ssd1351::SSD1351<Color, ssd1351::NoBuffer, 128, 96>();
auto display = ssd1351::SSD1351<Color, ssd1351::SingleBuffer, 128, 128>();

const uint8_t PROGMEM gamma8[] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
  10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
  17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
  25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
  37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
  51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
  69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
  90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
  115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
  144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
  177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
  215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255
};

void setup(void) {

  delay(3000); // 3 second delay for recovery
  make_mtable();

  display.begin();
  display.setTextSize(1);

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  Serial.begin(38400);

}

char teststr[30];
char timestr[30];




void updateOLED(uint32_t sscroll) {
  display.fillScreen(ssd1351::RGB());

  //display.setFont(FreeMonoBold24pt7b);
  display.setFont(FreeSansBoldOblique12pt7b);
  display.setTextSize(1);


  sprintf(teststr, "%ld", sscroll);
  timestr_for_index(sscroll);
  uint16_t w = display.getTextWidth(timestr);

  display.setCursor(64 - w / 2, 40);
  display.setTextColor(ssd1351::RGB(255, 140, 0));
  display.print(timestr);
  display.drawLine(0, 0, 127, 0, ssd1351::RGB(0, 0, 255));
  display.drawLine(0, 0, 0, 127, ssd1351::RGB(0, 0, 255));
  display.drawLine(127, 0, 127, 127, ssd1351::RGB(0, 0, 255));
  display.drawLine(0, 127, 127, 127, ssd1351::RGB(0, 0, 255));

  display.updateScreen();

}


void updateLEDs(uint32_t cptr) {
  if (cptr >= CTAB_LEN) {
    cptr = CTAB_LEN - 1;
  }
  if (cptr < 0) {
    cptr = 0;
  }

  //uint32_t ccolor =  pgm_read_dword(&ctable[cptr]);
  uint32_t ccolor =  mtable[cptr];
  uint8_t r = pgm_read_byte(&gamma8[ccolor >> 16]);
  uint8_t g = pgm_read_byte(&gamma8[ccolor >> 8  & 0xFF]);
  uint8_t b = pgm_read_byte(&gamma8[ccolor & 0xFF]);
  //FastLED.showColor(CRGB(r,g,b));
  FastLED.showColor(ccolor);
  //display.fillScreen(ssd1351::RGB(r, g, b));
  //display.updateScreen();

}



void hypupdateLEDs(uint32_t cptr) {

  // hyperbolically map cptr

  if (cptr >= CTAB_LEN) {
    cptr = CTAB_LEN - 1;
  }
  if (cptr < 0) {
    cptr = 0;
  }

  for (int i = 0; i < NUM_LEDS; i++) {

    leds[i] = CRGB(0, 0, 0);

    int j = hyp_map(NUM_LEDS, i, float(cptr) / float(CTAB_LEN), 5.0);


    //uint32_t ccolor =  pgm_read_dword(&ctable[cptr]);

    j = (i + cptr) % NUM_LEDS;
    uint32_t ccolor =  mtable[j];
    uint8_t r = pgm_read_byte(&gamma8[ccolor >> 16]);
    uint8_t g = pgm_read_byte(&gamma8[ccolor >> 8  & 0xFF]);
    uint8_t b = pgm_read_byte(&gamma8[ccolor & 0xFF]);

    leds[j] = CRGB(r, g, b);

    //FastLED.showColor(CRGB(r,g,b));

  }

  FastLED.show();


  //display.fillScreen(ssd1351::RGB(r, g, b));
  //display.updateScreen();

}


void hypupdate(uint32_t cptr) {

  // hyperbolically map cptr

  if (cptr >= CTAB_LEN) {
    cptr = CTAB_LEN - 1;
  }
  if (cptr < 0) {
    cptr = 0;
  }

  for (int i = 0; i < NUM_LEDS; i++) {


    int j = hyp_map(NUM_LEDS, i, float(cptr) / float(CTAB_LEN), 10.0);


  
    //j = (i + cptr) % NUM_LEDS;
    uint32_t ccolor =  mtable[j];
    if (ccolor) {
      Serial.write('x');
    }
    else {
      Serial.write('-');
    }

  }

  Serial.println("");

}

int hyp_map(int i, int n, float hcent, float zoom) {
  //hyperbolic map: returns int i 0<i<n  where i is hyperbolically mapped with center at fraction c 0.< c <1.
  // and zoomed'''

  float x = zoom * ((float(i) / n - 0.5) + (0.5 - hcent) );
  float y = (exp(2 * x) - 1) / (exp(2 * x) + 1 );
  // y is now -1<0<1, map to 0-> and scale n

  return int((y + 1) * (n / 2. - 1));
}

void timestr_for_index(long int idx) {
  // compute seconds since since midnight (constant from colors.h)


  float ssm = idx *  SECS_PER_C;

  int secs = (int) round(ssm);

  int hrs = secs / 3600;

  secs = secs - 3600 * hrs;

  int mins = secs / 60;
  secs = secs - 60 * mins;

  sprintf(timestr, "%02d:%02d:%02d", hrs, mins, secs);
  //sprintf(timestr, "%ld", idx);



}

void make_mtable() {
  int i;
  for (i = 0; i < CTAB_LEN; i++) {
    if ( (i >= 29) && ( i < 31)) {
      mtable[i] = 0x00008F;
    }
    else {
      mtable[i] = 0x000000;
    }
  }
}




void loop(void) {

  long int wptr = wheel.read();
  // prevent negative values
  if (wptr < 0) {



    wptr = 0;
    wheel.write(0);
  }

  //wptr = wptr >> 6;

  wptr = wptr >> 8;


  Serial.println(wptr);

  hypupdate(wptr);
  updateOLED(wptr);

  //FastLED.showColor(CHSV(hue, 255, 255));
  //delay(100);
}
