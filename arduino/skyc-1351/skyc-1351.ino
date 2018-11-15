

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


//#include "colors-119-10x.h"
#include "colors-59-10x.h"
//#include "smallcolors.h"

//#define SECS_PER_C 1.0
//#define CTAB_LEN 100



#include <FastLED.h>
#include <Encoder.h>
//#define HALF_STEP
#include "rotaryplus.h"


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
#define TOT_LEDS        104
#define RING_LEDS        60
#define MID_RING_START   60
#define MID_RING_LEN     32
#define IN_RING_START    92
#define IN_RING_LEN      12

CRGB leds[TOT_LEDS];

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

// debounced encoder for mechanical encoders
// (swapped for correct spin direction)
Rotary encr =  Rotary(ENCRB, ENCRA);
Rotary encl =  Rotary(ENCLB, ENCLA);


// keep encoder values so whe can tell when they have changed
long int tdial = 0;
long int rdial = 0;
long int ldial = 0;


Bounce leftb = Bounce(); // Instantiate a Bounce object
Bounce rightb = Bounce(); // Instantiate a Bounce object

int hyp_mode = 0; // switch between linear and hyperbolic mode


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
  215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 253, 254, 254
};



/********************************* Timing & state machine *****************************/


#define SHOW_TIME 0
#define SHOW_ZOOM 1
#define SHOW_SPEED 2
#define BLANK_SCREEN 3

int disp_state = SHOW_TIME;

// Use for display timeout
elapsedMillis display_time;

static unsigned long tick_interval = 1000; // ms
static unsigned long last_update_time = 0;


// seconds since midnight
int secs_sm = 0;

const int secs_per_day = 3600 * 24;

// global zoom (how much to expand hyperbolic zoom
const int MIN_ZOOM = 2;
const int MAX_ZOOM = 100;
const float ZOOM_SCALE = 10.;
// glob_soom is an int; divide by ZOOM_SCALE to get actual float zoom value fzoom
long  glob_zoom = 10;
float fzoom = 1.;

// global speed (how much to speed up time and time wheel)
#define MAX_SPEED 50
#define MIN_SPEED 1
int glob_speed = 7;


// strings for display
char disp_str[30];
char timestr[30];

void check_encl() {
  encl.process();
}
void check_encr() {
  encr.process();
}


void setup(void) {

  delay(3000); // 3 second delay for recovery

  // Turn on pullups on encoders and pushbuttons
  pinMode(ENCRA, INPUT_PULLUP);
  pinMode(ENCRB, INPUT_PULLUP);
  pinMode(ENCLA, INPUT_PULLUP);
  pinMode(ENCLB, INPUT_PULLUP);
  pinMode(RIGHTB, INPUT_PULLUP);
  pinMode(LEFTB, INPUT_PULLUP);

  encl.setPos(MIN_ZOOM);
  encr.setPos(MIN_SPEED);

  attachInterrupt(ENCLA, check_encl, CHANGE);
  attachInterrupt(ENCLB, check_encl, CHANGE);
  attachInterrupt(ENCRA, check_encr, CHANGE);
  attachInterrupt(ENCRB, check_encr, CHANGE);


  // pushbottons on encoders
  leftb.attach(LEFTB);
  leftb.interval(25);
  rightb.attach(RIGHTB);
  rightb.interval(25);




  display.begin();
  display.setTextSize(1);

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, TOT_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  Serial.begin(38400);

  set_defaults();
}

void set_defaults() {
  // set default speeds and positions

  // set local time to noon
  secs_sm = secs_per_day / 2;
  tdial = secs_sm;
  wheel.write(secs_sm);

  // set zoom to 1x
  encl.setPos((int)ZOOM_SCALE);
  fzoom = 1.0;

  // set speed to 5x
  encr.setPos(10);

}



void updateOLED(void) {
  // clear screen so we can overwrite
  display.fillScreen(ssd1351::RGB());
  switch (disp_state) {
    case SHOW_TIME:
      OLED_show_time();
      break;

    case SHOW_ZOOM:
      OLED_show_zoom();
      break;

    case SHOW_SPEED:
      OLED_show_speed();
      break;

    case BLANK_SCREEN:
      // timeout; blank screeen
      break;
  }

  display.updateScreen();
}

void OLED_show_time(void) {

  // global time is in secs_sm
  static int max_w = -1;


  //display.setFont(FreeMonoBold24pt7b);
  display.setFont(FreeSansBoldOblique12pt7b);
  display.setTextSize(1);

  timestr_for_seconds(secs_sm);
  // stop disply jitter by usning longest string so far
  if (display.getTextWidth(timestr) > max_w) {
    max_w = display.getTextWidth(timestr);
  }

  display.setCursor(64 - max_w / 2, 40);
  display.setTextColor(ssd1351::RGB(255, 140, 0));
  display.print(timestr);
  OLED_draw_frame(0, 0, 255);
}

void OLED_draw_frame(uint8_t r, uint8_t g, uint8_t b ) {
  display.drawLine(0, 0, 127, 0, ssd1351::RGB(r, g, b));
  display.drawLine(0, 0, 0, 127, ssd1351::RGB(r, g, b));
  display.drawLine(127, 0, 127, 127, ssd1351::RGB(r, g, b));
  display.drawLine(0, 127, 127, 127, ssd1351::RGB(r, g, b));

}

void OLED_show_zoom(void) {

  //display.setFont(FreeMonoBold24pt7b);
  display.setFont(FreeSansBoldOblique12pt7b);
  display.setTextSize(1);


  display.setTextColor(ssd1351::RGB(255, 140, 0));
  display.setCursor(32, 40);
  display.print("zoom:");

  sprintf(disp_str, "%3.1fx", fzoom);

  display.setCursor(40, 70);
  display.print(disp_str);

  OLED_draw_frame(0, 255, 255);
}


void OLED_show_speed(void) {

  //display.setFont(FreeMonoBold24pt7b);
  display.setFont(FreeSansBoldOblique12pt7b);
  display.setTextSize(1);


  display.setCursor(32, 40);
  display.setTextColor(ssd1351::RGB(255, 140, 0));
  display.print("speed:");

  OLED_draw_frame(255, 0, 255);


  display.setCursor(40, 70);
  sprintf(disp_str, "%ldx", glob_speed);
  display.print(disp_str);
}





void updateLEDs(int secs, float zoom) {

  int cptr = index_for_secs(secs);
  // hyperbolically map cptr
  int j = 0;
  float hcent = float(cptr) / float(CTAB_LEN);

  //Serial.println(cptr);
  if (hcent > 1.) {
    hcent = 1.;
  }
  else if (hcent < 0.0) {
    hcent = 0.;
  }


  for (int i = 0; i < RING_LEDS; i++) {


  
    leds[i] = CRGB(0, 0, 0);

    // reverse map so clockwise when looking from front
    if (hyp_mode) {
      j = ihyp_map(RING_LEDS - i -1, RING_LEDS, CTAB_LEN, hcent, 10 * zoom);
    }
    else {
      j = ilin_map(RING_LEDS - i -1, RING_LEDS, CTAB_LEN, hcent, zoom);

    }
    uint32_t ccolor =  (uint32_t)pgm_read_dword(&ctable[j]);

    uint8_t r = pgm_read_byte(&gamma8[(ccolor >> 16) & 0xFF]);
    uint8_t g = pgm_read_byte(&gamma8[(ccolor >> 8)  & 0xFF]);
    uint8_t b = pgm_read_byte(&gamma8[ccolor & 0xFF]);

    /*
        Serial.print(i);
        Serial.print(":");
        Serial.print(j);
        Serial.print(">");
        char temps[32];
        sprintf(temps, "%x", ccolor);
        Serial.print(temps);
        Serial.print("=");
        Serial.print(r);
        Serial.print(" ");
        Serial.print(g);
        Serial.print(" ");
        Serial.print(b);
        Serial.print("\n");
    */

    //leds[i] = CRGB(r + 1, g + 2, b + 2);
    leds[i] = CRGB(r, g, b);
  }

  CHSV hsv = rgb2hsv_approximate( leds[20] );

  for (int i = MID_RING_START; i < MID_RING_START + MID_RING_LEN; i++) {
    //leds[i] = CHSV(hsv.h + 128, hsv.s, hsv.v);
  }


  for (int i = IN_RING_START; i < IN_RING_START + IN_RING_LEN; i++) {

    //leds[i] = leds[20];

  }

  FastLED.show();
}


int index_for_secs(int secs) {
  // return index into color table given seconds after midnight
  //Serial.print("-");
  //Serial.print(secs);
  //Serial.print("-");
  //Serial.print(secs_per_day);
  //Serial.print(">");
  float dayfrac = float(secs) / float(secs_per_day);
  int idx = (int) (dayfrac * float(CTAB_LEN));
  if (idx >=  CTAB_LEN) {
    idx = CTAB_LEN - 1;
  }
  return (idx);
}

int secs_for_index(int idx) {
  // return index into color table given seconds after midnight
  float dayfrac = float(idx) / float(CTAB_LEN);
  int secs = (int) floor(dayfrac * secs_per_day);
  return (secs);

}

void timestr_for_seconds(int secs) {
  // compute seconds since since midnight (constant from colors.h)

  int hrs = secs / 3600;

  secs = secs - 3600 * hrs;

  int mins = secs / 60;
  secs = secs - 60 * mins;

  sprintf(timestr, "%02d:%02d:%02d", hrs, mins, secs);
  //sprintf(timestr, "%ld", idx);

}

void update_time() {

  long int scaled_interval = 1000 / glob_speed;
  if (millis() - last_update_time >= scaled_interval) {
    last_update_time += scaled_interval;
    //Serial.println(scaled_interval);
    secs_sm += 1;
    wrap_time();
  }
}


void wrap_time() {
  // Keep global time within a 24 hour period
  if (secs_sm >  secs_per_day) {
    secs_sm -= secs_per_day;
  }
  else if (secs_sm <  0) {
    secs_sm += secs_per_day;
  }
}

float pmap(float x, float inmin, float inmax, float outmin, float outmax) {
  // processing-like map function
  // first map to 0-1
  float y = (x - inmin) / (inmax - inmin);
  // now scale to output
  return (y * (outmax - outmin) + outmin);
}


int ilin_map(int i, int n, int m, float hcent, float zoom) {
  // inverse linear map into an output table of n slots from an input map of m slots.
  // hcent is 0 < hcent < 1 maps center of transformation.
  // wraparound occurs. Zoom is roughly slope.
  float y = (float(i - n / 2) / float(n)) / zoom;

  // y is now symmetric around zero, recenter, scale to m, and clip
  int j = (int) m * (y + hcent);

  if (j >= m) {
    j = m;
  }
  else if (j < 0) {
    j = 0;
  }

  return (j);
}

int ihyp_map(int o, int n, int m, float hcent, float zoom) {
  // use this to nonlinear map (inverse hyperbolic tangent) into an output table of n slots from an input map of m slots.
  // hcent is 0 < hcent < 1 maps center of transformation. Zoom is roughly slope.
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

  //  float q = (zoom * 0.5 * float(m) * y) + (hcent * float(m));
  float q = (float(m) * y) / zoom + (hcent * float(m));

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

  // increment currently displayed time
  update_time();

  leftb.update();
  rightb.update();

  if ( leftb.fell()) {
    display_time = 0;
    // toggle between linear and hyperbolic
    hyp_mode = 1 - hyp_mode;
    Serial.print("hyp mode: ");
    Serial.println(hyp_mode);
  }

  if ( rightb.fell()) {
    Serial.println("Defaults!");
    set_defaults();
    display_time = 0;
  }


  if (encl.change()) {
    display_time = 0;
    disp_state = SHOW_ZOOM;
    glob_zoom = encl.pos();
    if (glob_zoom > MAX_ZOOM) {
      glob_zoom = MAX_ZOOM;
      encl.setPos(MAX_ZOOM);
    }
    else if (glob_zoom < MIN_ZOOM) {
      glob_zoom = MIN_ZOOM;
      encl.setPos(MIN_ZOOM);
    }
    fzoom = float(glob_zoom) / ZOOM_SCALE;
  }

  if (encr.change()) {
    display_time = 0;
    disp_state = SHOW_SPEED;
    glob_speed = encr.pos();
    if (glob_speed > MAX_SPEED) {
      glob_speed = MAX_SPEED;
      encr.setPos(MAX_SPEED);
    }
    else if (glob_speed < MIN_SPEED) {
      glob_speed = MIN_SPEED;
      encr.setPos(MIN_SPEED);
    }
  }

  // wheel difference since last time through loop
  int tdiff = wheel.read() - tdial;
  if (tdiff != 0) { // if it's changed
    tdial = wheel.read();
    display_time = 0;
    disp_state = SHOW_TIME;
    // change time by adding offset from dial. Scale it because it is high resolution.
    secs_sm += glob_speed * (tdiff / 8);
    wrap_time();
  }


  updateLEDs(secs_sm, fzoom);

  // timout display screen after a given number of seconds
  if (display_time > 2500) {
    disp_state = SHOW_TIME;
  }
  else if (display_time > 30000) {
    disp_state = BLANK_SCREEN;
  }

  updateOLED();

}

float mymap(float x, float in_min, float in_max, float out_min, float out_max) {
  float y = (x - in_min) / (in_max - in_min);
  return (y * (out_max - out_min) + out_min);
}


/*
   Debug routines used in development...


   // for debugging hyperbolic map
  #define MTAB_LEN (60)
  uint32_t mtable[MTAB_LEN];
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



*/
