#include <Arduino.h>
#include <SPI.h>
#include "canInit.h"
#include "init_screen.h"
#include "boost_gauge.h"
#include "globals.h"

int global_screen_value = 3; 
volatile bool setupInitScreen = true;

U8G2_SSD1306_128X64_NONAME_1_4W_SW_SPI u8g2(U8G2_R0, 
                                            /* clock/D0=*/ 13, 
                                            /* data/D1=*/ 14, 
                                            /* cs=*/ 17, 
                                            /* dc=*/ 16, 
                                            /* reset=*/ 15);


unsigned long oilPressScreenTimer = 0;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  delay(400);

  Serial.println("INITIALIZING SCREEN");
  initScreen();
  delay(400);

  Serial.println("INITIALIZING CAN");
  initCanT4();
  delay(200);

  // Display Boot Logo
  Serial.println("DRAW BOOT LOGO");

  drawBootLogo(); //make draw the boot logo
  delay(1000); // Sleep for 3 seconds to give boot illusion
  
  setupInitScreen = true;

  oilPressScreenTimer = millis();

  //while(millis() - oilPressScreenTimer < 30000){
  //    Serial.println("Startup Oil Pressure!!");
  //    switchScreens(2);
  //}

}

// the loop function runs over and over again forever
void loop() {
  // Set default screen to 1/BOOST
  switchScreens(global_screen_value);
  can1.events(); //Need to check can events in buffer.
  readCanMessages();
  checkSnooze();
  //Serial.println("FINISHEDMAINLOOP!!!");
}