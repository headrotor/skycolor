

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
    Pin 18 Enc L Button (yel)
    Pin 19 Enc L PHA (grn)
    Pin 20 Enc L PHB (blu)
    Pin 21 Enc R Button (yel)
    Pin 22 Enc R PHA (grn)
    Pin 23 Enc R PHB (blu)

*/

#include <Arduino.h>


#include "smallcolors.h"
//#define SECS_PER_C 1.0
//#define CTAB_LEN 100

// for debugging hyperbolic map
#define MTAB_LEN (60)
uint32_t mtable[MTAB_LEN];



#include <FastLED.h>
#include <Encoder.h>
#include <SPI.h>
#define SLOW_SPI
#include <ssd1351.h>
#include <Bounce2.h>



/***********************************************FASTLED***********************************/




FASTLED_USING_NAMESPACE

#define DATA_PIN    6
//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
//#define NUM_LEDS    (32+24+16+12 +8 + 1 + 10)
#define NUM_LEDS    60

CRGB leds[NUM_LEDS];

#define BRIGHTNESS          255
#define FRAMES_PER_SECOND  120



/****************************************** ENCODERS & BUTTONS********************/


// # need to turn on pullups so used #defs

#define RIGHTB 18
#define ENCRA  19
#define ENCRB  20


#define LEFTB  21
#define ENCLA  22
#define ENCLB  23


Encoder wheel(2, 3);  // time wheel
Encoder encl(ENCLA, ENCLB); //left dial encoder
Encoder encr(ENCRA, ENCRB); //right dial encoder

// keep encoder values so whe can tell when they have changed
long int tdial = 0;
long int rdial = 0;
long int ldial = 0;


Bounce leftb = Bounce(); // Instantiate a Bounce object
Bounce rightb = Bounce(); // Instantiate a Bounce object

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

  // Turn on pullups on encoders and pushbuttons
  pinMode(ENCRA, INPUT_PULLUP);
  pinMode(ENCRB, INPUT_PULLUP);
  pinMode(ENCLA, INPUT_PULLUP);


  // pushbottons on encoders
  leftb.attach(LEFTB);
  leftb.interval(25);
  rightb.attach(RIGHTB);
  rightb.interval(25);

  pinMode(LEFTB, INPUT_PULLUP);
  pinMode(RIGHTB, INPUT_PULLUP);



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


void updateLEDs(int cptr) {
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



void hypupdateLEDs(int cptr, float zoom) {

  // hyperbolically map cptr
  int j = 0;
  float hcent = float(cptr) / float(CTAB_LEN);

  if (hcent > 1.) {
    hcent = 1.;
  }
  else if (hcent < 0.0) {
    hcent = 0.;
  }


  for (int i = 0; i < NUM_LEDS; i++) {

    leds[i] = CRGB(0, 0, 0);

    j = ihyp_map(i, NUM_LEDS, CTAB_LEN, hcent, zoom);


    uint32_t ccolor =  pgm_read_dword(&ctable[j]);

    uint8_t r = pgm_read_byte(&gamma8[ccolor >> 16]);
    uint8_t g = pgm_read_byte(&gamma8[ccolor >> 8  & 0xFF]);
    uint8_t b = pgm_read_byte(&gamma8[ccolor & 0xFF]);

    leds[i] = CRGB(r, g, b);
  }

  FastLED.show();

}


void hypupdate( int mptr, float zoom) {

  // hyperbolically map cptr
  int j = 0;

  float hcent = float(mptr) / float(MTAB_LEN);

  if (hcent > 1.) {
    hcent = 1.;
  }
  else if (hcent < 0.0) {
    hcent = 0.;
  }

  /*

    Serial.print(" hcent=");
    Serial.print(hcent );

    Serial.print(" mptr=");
    Serial.print(mptr);
  */

  for (int i = 0; i < MTAB_LEN; i++) {


    j = ihyp_map( i, MTAB_LEN, MTAB_LEN, hcent, zoom);


    //j = abs(j);
    // Serial.print(j);
    //Serial.print(' ');

    if (j >= 0) {
      uint32_t ccolor =  mtable[j];
      if (ccolor) {
        Serial.write('x');
      }
      else {
        Serial.write('-');
      }

    }
  }
  Serial.println("");

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
  for (i = 0; i < MTAB_LEN; i++) {
    if ( (i >= 29) && ( i < 31)) {
      mtable[i] = 0x00008F;
    }
    else {
      mtable[i] = 0x000000;
    }
  }
}

int ihyp_map(int o, int n, int m, float hcent, float zoom) {
  // use this to nonlinear map into an output table of n slots from an input map of m slots.
  // hcent is 0 < hcent < maps center of transformation. Zoom is roughly slope.
  // i is output element out of n.
  //  m is total number of input slots. (Range is infinite but clipped to 0 < hcent*m < m
  float eps = 0.001;
  float x = 2. * (float(o) / float(n))  - 1.0;
  if (x <= -1.0) {
    x = -1.0 + eps;
  }
  else if  (x >= 1.0) {
    x = 1.0 - eps;
  }
  float y = 2 * log((1 + x) / (1 - x));
  // y is now symmetric around zero, recenter, scale to m, and clip


  if (hcent < 0) {
    hcent = 0;
  }
  else if  (hcent >= 1.0) {
    hcent = 1.0;
  }


  float q = (zoom * 0.5 * float(m) * y) + (hcent * float(m));

  /*
    Serial.print(" q=");
    Serial.print(q);
    Serial.print("q ");
  */
  if (q >= float(m)) {
    return (m - 1);
  }
  else if  (q < 0.) {
    return (int)(0);
  }

  else return ((int) floor(q));
}


void loop(void) {

  leftb.update();
  rightb.update();

  if ( leftb.fell()) {
    Serial.println("LEFTB!");
  }

  if ( rightb.fell()) {
    Serial.println("rightb!");
  }


  if (encl.read() != ldial) {
    ldial = encl.read();
    Serial.print("ldiall: ");
    Serial.println(ldial);

  }
  if (encr.read() != rdial) {
    rdial = encr.read();
    Serial.print("rdiall: ");
    Serial.println(rdial);

  }

  long int wptr = wheel.read();
  // prevent negative values
  if (wptr < 0) {

    wptr = 0;
    wheel.write(0);
  }

  //wptr = wptr >> 6;

  wptr = wptr >> 8;

  hypupdate((int)(wptr & 0xFFFF), 0.05);
  hypupdateLEDs((int)(wptr & 0xFFFF), 0.05);


  updateOLED(wptr);
}


/*
  int hyp_map(int i, int n, float hcent, float zoom) {
  //hyperbolic map: returns int i 0<i<n  where i is hyperbolically mapped with center at fraction c 0.< c <1.
  // and zoomed'''

  float x = zoom * ((float(i) / n - 0.5) + (0.5 - hcent) );
  float y = (exp(2 * x) - 1) / (exp(2 * x) + 1 );
  // y is now -1<0<1, map to 0-> and scale n

  return int((y + 1) * (n / 2. - 1));
  }
*/
