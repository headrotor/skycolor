/*

  HelloWorld.ino

  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright (c) 2016, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification,
  are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice, this list
    of conditions and the following disclaimer.

    Redistributions in binary form must reproduce the above copyright notice, this
    list of conditions and the following disclaimer in the documentation and/or other
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <Arduino.h>
#include <U8g2lib.h>



#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#include <Encoder.h>

// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder wheel(2, 3);


// works 2 fps
//U8G2_SSD1327_EA_W128128_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

//works 36-17 fps
//U8G2_SSD1327_EA_W128128_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//works
U8G2_SSD1327_MIDAS_128X128_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

// works ***************
//U8X8_SSD1327_MIDAS_128X128_HW_I2C u8x8(U8X8_PIN_NONE);


//WORKS! I2C, 09.fps SLOW******************
//U8G2_SSD1327_MIDAS_128X128_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);

// works,  I2C, 0.9 fps
//U8G2_SSD1327_MIDAS_128X128_F_HW_I2C u8g2(U8G2_R0, /*reset=*/ U8X8_PIN_NONE  );


#include <FastLED.h>

FASTLED_USING_NAMESPACE

#define DATA_PIN    5
//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    (32+24+16+12 +8 + 1 + 10)
//#define NUM_LEDS    32

CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120




void setup(void) {
  u8g2.begin();
  delay(3000); // 3 second delay for recovery

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  Serial.begin(38400);

  initOLED();
}

void initOLEDOLD(void) {
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_profont15_tf);  // choose a suitable font
  //u8g2.setFont(u8g2_font_ncenB08_tr);  // choose a suitable font
  //u8g2.drawStr(0, 17, "Hello World!");  // write something to the internal memory
  //u8g2.setDrawColor(1);
  //u8g2.drawBox(0, 0, u8g2.getDisplayHeight(), u8g2.getDisplayWidth());
  u8g2.setContrast(128);
  u8g2.setFontMode(0);
  u8g2.setDrawColor(128);
  u8g2.drawStr(3, 15, "Color=1, Mode 0");
  int dheight = 128; // u8g2.getDisplayHeight()
  int dwidth = 95; // u8g2.getDisplayWidth()
  u8g2.drawFrame(0, 0, dheight - 1, dwidth);
  u8g2.drawFrame(5, 5, dheight - 11, dwidth - 10);
  u8g2.drawFrame(10, 10, dheight - 21, dwidth - 20);
  u8g2.drawFrame(15, 15, dheight - 31, dwidth - 30);

  u8g2.setCursor(3, 75);
  u8g2.print("hello");

  u8g2.setDrawColor(0);


  u8g2.setContrast(255);
  u8g2.drawStr(3, 30, "Color=0, Mode 0");
  u8g2.setFontMode(1);
  u8g2.setDrawColor(10);
  u8g2.drawStr(3, 45, "Color=1, Mode 1");
  u8g2.setDrawColor(200);
  u8g2.drawStr(3, 60, "Color=0, Mode 1");
  u8g2.sendBuffer();          // transfer internal memory to the display

  // reset font mode and color
  u8g2.setFontMode(0);
  u8g2.setDrawColor(255);
}

void initOLED(void) {
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_profont15_tf);  // choose a suitable font
  //u8g2.setFont(u8g2_font_ncenB08_tr);  // choose a suitable font
  //u8g2.drawStr(0, 17, "Hello World!");  // write something to the internal memory
  //u8g2.setDrawColor(1);
  //u8g2.drawBox(0, 0, u8g2.getDisplayHeight(), u8g2.getDisplayWidth());
  u8g2.setContrast(255);
  //u8g2.setDrawColor(255);
  u8g2.drawStr(3, 15, "Color=1, Mode 0");
  int dheight =  u8g2.getDisplayHeight();
  int dwidth =  u8g2.getDisplayWidth();
  u8g2.drawFrame(0, 0, dwidth-1, dheight - 1 );  
  u8g2.drawFrame(0, 0, dwidth-1, dheight - 10 );
  u8g2.drawFrame(0, 0, dwidth-1, 96 );

  u8g2.setCursor(3, 75);
  u8g2.print("hello");
  u8g2.sendBuffer();          // transfer internal memory to the display
}

char ostr[32];

void loop(void) {
  sprintf(ostr, "pos: %d", wheel.read());

  int hue = (wheel.read() >> 8) & 255;
  Serial.println(hue);
  FastLED.showColor(CHSV(hue, 255, 255));
  u8g2.setCursor(3, 75);
    u8g2.setDrawColor(255);
  u8g2.print(ostr);
  u8g2.setContrast(255);
  u8g2.drawStr(3, 75, ostr );

  u8g2.sendBuffer();          // transfer internal memory to the display
  //delay(100);
}
