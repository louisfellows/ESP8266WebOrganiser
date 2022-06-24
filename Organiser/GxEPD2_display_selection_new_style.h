// Display Library example for SPI e-paper panels from Dalian Good Display and boards from Waveshare.
// Requires HW SPI and Adafruit_GFX. Caution: the e-paper panels require 3.3V supply AND data lines!
//
// Display Library based on Demo Example from Good Display: http://www.e-paper-display.com/download_list/downloadcategoryid=34&isMode=false.html
//
// Author: Jean-Marc Zingg
//
// Version: see library.properties
//
// Library: https://github.com/ZinggJM/GxEPD2

// Supporting Arduino Forum Topics:
// Waveshare e-paper displays with SPI: http://forum.arduino.cc/index.php?topic=487007.0
// Good Display ePaper for Arduino: https://forum.arduino.cc/index.php?topic=436411.0

// select the display class (only one), matching the kind of display panel
#define GxEPD2_DISPLAY_CLASS GxEPD2_BW
//#define GxEPD2_DISPLAY_CLASS GxEPD2_3C
//#define GxEPD2_DISPLAY_CLASS GxEPD2_7C

// select the display driver class (only one) for your  panel
//#define GxEPD2_DRIVER_CLASS GxEPD2_102     // GxEPD2_102   80x128, UC8175
//#define GxEPD2_DRIVER_CLASS GxEPD2_154     // GDEP015OC1  200x200, IL3829, no longer available
//#define GxEPD2_DRIVER_CLASS GxEPD2_154_D67 // GDEH0154D67 200x200, SSD1681
//#define GxEPD2_DRIVER_CLASS GxEPD2_154_T8  // GDEW0154T8  152x152, UC8151 (IL0373)
//#define GxEPD2_DRIVER_CLASS GxEPD2_154_M09 // GDEW0154M09 200x200, JD79653A
//#define GxEPD2_DRIVER_CLASS GxEPD2_154_M10 // GDEW0154M10 152x152, UC8151D
//#define GxEPD2_DRIVER_CLASS GxEPD2_213     // GDE0213B1   128x250, IL3895, phased out
//#define GxEPD2_DRIVER_CLASS GxEPD2_213_B72 // GDEH0213B72 128x250, SSD1675A (IL3897)
//#define GxEPD2_DRIVER_CLASS GxEPD2_213_B73 // GDEH0213B73 128x250, SSD1675B
//#define GxEPD2_DRIVER_CLASS GxEPD2_213_B74 // GDEM0213B74 128x250, SSD1680
//#define GxEPD2_DRIVER_CLASS GxEPD2_213_flex // GDEW0213I5F 104x212, UC8151 (IL0373)
//#define GxEPD2_DRIVER_CLASS GxEPD2_213_M21 // GDEW0213M21 104x212, UC8151 (IL0373)
//#define GxEPD2_DRIVER_CLASS GxEPD2_213_T5D // GDEW0213T5D 104x212, UC8151D
//#define GxEPD2_DRIVER_CLASS GxEPD2_290     // GDEH029A1   128x296, SSD1608 (IL3820)
//#define GxEPD2_DRIVER_CLASS GxEPD2_290_T5  // GDEW029T5   128x296, UC8151 (IL0373)
//#define GxEPD2_DRIVER_CLASS GxEPD2_290_T5D // GDEW029T5D  128x296, UC8151D
//#define GxEPD2_DRIVER_CLASS GxEPD2_290_I6FD // GDEW029I6FD  128x296, UC8151D
//#define GxEPD2_DRIVER_CLASS GxEPD2_290_T94 // GDEM029T94  128x296, SSD1680
//#define GxEPD2_DRIVER_CLASS GxEPD2_290_T94_V2 // GDEM029T94  128x296, SSD1680, Waveshare 2.9" V2 variant
//#define GxEPD2_DRIVER_CLASS GxEPD2_290_M06 // GDEW029M06  128x296, UC8151D
//#define GxEPD2_DRIVER_CLASS GxEPD2_260     // GDEW026T0   152x296, UC8151 (IL0373)
//#define GxEPD2_DRIVER_CLASS GxEPD2_260_M01 // GDEW026M01  152x296, UC8151 (IL0373)
//#define GxEPD2_DRIVER_CLASS GxEPD2_270     // GDEW027W3   176x264, EK79652 (IL91874)
//#define GxEPD2_DRIVER_CLASS GxEPD2_371     // GDEW0371W7  240x416, UC8171 (IL0324)
//#define GxEPD2_DRIVER_CLASS GxEPD2_370_TC1 // ED037TC1  280x480, SSD1677, Waveshare 3.7"
//#define GxEPD2_DRIVER_CLASS GxEPD2_420     // GDEW042T2   400x300, UC8176 (IL0398)
//#define GxEPD2_DRIVER_CLASS GxEPD2_420_M01 // GDEW042M01  400x300, UC8176 (IL0398)
//#define GxEPD2_DRIVER_CLASS GxEPD2_583     // GDEW0583T7  600x448, UC8179 (IL0371)
#define GxEPD2_DRIVER_CLASS GxEPD2_583_T8  // GDEW0583T8  648x480, GD7965
//#define GxEPD2_DRIVER_CLASS GxEPD2_750     // GDEW075T8   640x384, UC8179 (IL0371)
//#define GxEPD2_DRIVER_CLASS GxEPD2_750_T7  // GDEW075T7   800x480, GD7965
//#define GxEPD2_DRIVER_CLASS GxEPD2_1160_T91 // GDEH116T91 960x640, SSD1677
//#define GxEPD2_DRIVER_CLASS GxEPD2_1248     // GDEW1248T3  1304x984, UC8179
// 3-color e-papers
//#define GxEPD2_DRIVER_CLASS GxEPD2_154c     // GDEW0154Z04 200x200, IL0376F, no longer available
//#define GxEPD2_DRIVER_CLASS GxEPD2_154_Z90c // GDEH0154Z90 200x200, SSD1681
//#define GxEPD2_DRIVER_CLASS GxEPD2_213c     // GDEW0213Z16 104x212, UC8151 (IL0373)
//#define GxEPD2_DRIVER_CLASS GxEPD2_213_Z19c // GDEW0213Z19 104x212, UC8151D
//#define GxEPD2_DRIVER_CLASS GxEPD2_213_Z98c // GDEY0213Z98 122x250, SSD1680
//#define GxEPD2_DRIVER_CLASS GxEPD2_290c     // GDEW029Z10  128x296, UC8151 (IL0373)
//#define GxEPD2_DRIVER_CLASS GxEPD2_290_Z13c // GDEH029Z13  128x296, UC8151D
//#define GxEPD2_DRIVER_CLASS GxEPD2_290_C90c // GDEM029C90  128x296, SSD1680
//#define GxEPD2_DRIVER_CLASS GxEPD2_266c     // GDEY0266Z90 152x296, SSD1680
//#define GxEPD2_DRIVER_CLASS GxEPD2_270c     // GDEW027C44  176x264, IL91874
//#define GxEPD2_DRIVER_CLASS GxEPD2_420c     // GDEW042Z15  400x300, UC8176 (IL0398)
//#define GxEPD2_DRIVER_CLASS GxEPD2_420c_Z21 // GDEQ042Z21  400x300, UC8276
//#define GxEPD2_DRIVER_CLASS GxEPD2_583c     // GDEW0583Z21 600x448, UC8179 (IL0371)
//#define GxEPD2_DRIVER_CLASS GxEPD2_583c_Z83 // GDEW0583Z83 648x480, GD7965
//#define GxEPD2_DRIVER_CLASS GxEPD2_750c     // GDEW075Z09  640x384, UC8179 (IL0371)
//#define GxEPD2_DRIVER_CLASS GxEPD2_750c_Z08 // GDEW075Z08  800x480, GD7965
//#define GxEPD2_DRIVER_CLASS GxEPD2_750c_Z90 // GDEH075Z90  880x528, SSD1677
// 7-color e-paper
//#define GxEPD2_DRIVER_CLASS GxEPD2_565c // Waveshare 5.65" 7-color (3C graphics)
// grey levels parallel IF e-papers on Waveshare e-Paper IT8951 Driver HAT
//#define GxEPD2_DRIVER_CLASS GxEPD2_it60           // ED060SCT 800x600
//#define GxEPD2_DRIVER_CLASS GxEPD2_it60_1448x1072 // ED060KC1 1448x1072
//#define GxEPD2_DRIVER_CLASS GxEPD2_it78_1872x1404 // ED078KC2 1872x1404

// SS is usually used for CS. define here for easy change
#ifndef EPD_CS
#define EPD_CS SS
#endif

#if defined(GxEPD2_DISPLAY_CLASS) && defined(GxEPD2_DRIVER_CLASS)

// somehow there should be an easier way to do this
#define GxEPD2_BW_IS_GxEPD2_BW true
#define GxEPD2_3C_IS_GxEPD2_3C true
#define GxEPD2_7C_IS_GxEPD2_7C true
#define GxEPD2_1248_IS_GxEPD2_1248 true
#define IS_GxEPD(c, x) (c##x)
#define IS_GxEPD2_BW(x) IS_GxEPD(GxEPD2_BW_IS_, x)
#define IS_GxEPD2_3C(x) IS_GxEPD(GxEPD2_3C_IS_, x)
#define IS_GxEPD2_7C(x) IS_GxEPD(GxEPD2_7C_IS_, x)
#define IS_GxEPD2_1248(x) IS_GxEPD(GxEPD2_1248_IS_, x)

#include "GxEPD2_selection_check.h"

#if defined (ESP8266)
#define MAX_DISPLAY_BUFFER_SIZE (81920ul-34000ul-5000ul) // ~34000 base use, change 5000 to your application use
#if IS_GxEPD2_BW(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8))
#elif IS_GxEPD2_3C(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8))
#elif IS_GxEPD2_7C(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE) / (EPD::WIDTH / 2) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE) / (EPD::WIDTH / 2))
#endif
// adapt the constructor parameters to your wiring
//GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=D8*/ EPD_CS, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4));
// mapping of Waveshare e-Paper ESP8266 Driver Board, new version
GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=15*/ EPD_CS, /*DC=4*/ 4, /*RST=2*/ 2, /*BUSY=5*/ 5));
// mapping of Waveshare e-Paper ESP8266 Driver Board, old version
//GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=15*/ EPD_CS, /*DC=4*/ 4, /*RST=5*/ 5, /*BUSY=16*/ 16));
#undef MAX_DISPLAY_BUFFER_SIZE
#undef MAX_HEIGHT
#endif

#endif
