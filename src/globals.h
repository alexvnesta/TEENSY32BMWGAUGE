#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>

extern int global_screen_value;
extern int dim;

extern int canReadFlag;

extern U8G2_SSD1306_128X64_NONAME_1_4W_SW_SPI u8g2;

void setup();

extern volatile bool setupInitScreen;

void initScreen();

#endif