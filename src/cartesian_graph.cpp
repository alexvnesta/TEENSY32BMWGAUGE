#include "globals.h"
#include <Arduino.h>
#include "cartesian_graph.h"
#include "parseCan.h"
#include <U8g2lib.h>


const int WIDTH=128;
const int HEIGHT=64;
const int LENGTH=WIDTH;

int xPosition;
int lineHorsePower[LENGTH];
int lineTorque[LENGTH];

float currentHorsePower = 0;

float maxHorsePower = 1;
float maxTorque = 1;
int maxrpm = 1;

int rpmcounter = 0;
int torquecounter = 0;

int resetTime = 0;


void clearLine(){
  for(int i=0; i<LENGTH; i++){
    lineHorsePower[i] = -1;
    lineTorque[i] = -1;
    maxHorsePower = 0;
    maxTorque = 0;
  }
}

void drawLine(){
  
  u8g2.setFont(u8g2_font_5x8_mf);

  u8g2.setCursor(14, 12);
  u8g2.print("TQ:");
  u8g2.setCursor(14, 24);
  u8g2.print("HP:");
  u8g2.setCursor(30, 12);
  u8g2.print(torquelbf);
  u8g2.setCursor(30, 24);
  u8g2.print(currentHorsePower);

float lastxvaltq = 0;
float lastxvalhp = 0;

for(int x = 0; x < xPosition; x++){
  if (lineTorque[x] > 0) lastxvaltq = lineTorque[x];
  if (lineTorque[x] <= 1) lineTorque[x] = lastxvaltq;
  
  if (lineHorsePower[x] > 0) lastxvalhp = lineHorsePower[x];
  if (lineHorsePower[x] <= 1) lineHorsePower[x] = lastxvalhp;

  u8g2.drawPixel(x + 13, HEIGHT - 13 - lineHorsePower[x]);

  if (x % 3 == 0){
  u8g2.drawPixel(x + 13, HEIGHT - 13 - lineTorque[x]);
  u8g2.drawPixel(x + 13, HEIGHT - 13 - lineTorque[x]+1);
  }
  }

}

void drawAxisX(){
  u8g2.drawHLine(12, HEIGHT - 12, WIDTH);
  int highestLabel = floor(maxrpm/1000) + 2;
  int currentLabelPosition = 0;
  u8g2.setFont(u8g2_font_6x10_mn);
  for(int currentLabel = 0; currentLabel < highestLabel; currentLabel++)
  {
      currentLabelPosition = map(currentLabel, 0, highestLabel, 0, 128);
        u8g2.setCursor(currentLabelPosition, 64);
        u8g2.print(currentLabel);
  } 
}

void drawAxisY(){
  u8g2.drawVLine(12, 0, HEIGHT-12);
  int highestLabel = floor(maxHorsePower/100) + 3;
  int currentLabelPosition = 0;
  u8g2.setFont(u8g2_font_6x10_mn);


  for(int currentLabel = 0; currentLabel < highestLabel; currentLabel++)
  {
    currentLabelPosition = map(currentLabel, 0, highestLabel, 64, 0);
    u8g2.setCursor(0, currentLabelPosition);
    u8g2.print(currentLabel);

  } 
}

void setupGraph(void) {
  Serial.println("Setting Up Cartesian Graph!!");
  xPosition = 0;
  rpm = 1;
  currentHorsePower = 1;
  torquelbf = 1;
  maxHorsePower = 1;
  maxTorque = 1;
  maxrpm = 1;
  rpmcounter = 0;
  torquecounter = 0;  
  clearLine();
}

void drawGraph(void){
  if (setupInitScreen == true){
    setupGraph();
    setupInitScreen = false;
  }

  Serial.print("CAN bus Torque lbf/nm:");
  Serial.print(torquelbf);
  Serial.print("/");
  Serial.println(torquenm);

  //torquelbf = torquelbf + random(20);
  //Serial.print("Changed Torque:");
  //Serial.println(torquelbf);

  //Serial.print("current RPM:");
  //Serial.println(rpm);

  //rpm += random(250);

/*
  if (speed == 0){
    u8g2.clear();
    u8g2.firstPage();
    do {
      u8g2.setFont(u8g2_font_freedoomr10_mu);
      u8g2.drawStr(0,20, "VIRTUAL DYNO");
      u8g2.drawStr(0,40, "GET READY!");
    } while(u8g2.nextPage() );
  }
  */

  //calculate horsepower
  if (rpm > 0 && torquelbf > 0) currentHorsePower = (rpm * torquelbf) / (60/(2*3.1415962))/1000;

  Serial.print("current Horsepower:");
  Serial.println(currentHorsePower);

  if(torquelbf > maxTorque){
    maxTorque = torquelbf;
  }

  if(currentHorsePower > maxHorsePower){
    maxHorsePower = currentHorsePower;
  }
  if(rpm > maxrpm){
    maxrpm = rpm;
  }

  xPosition = map(rpm, 0, 8000, 13, 128);

  lineTorque[xPosition] = map(torquelbf, 0, 600, 0, HEIGHT-13);
  lineHorsePower[xPosition] = map(currentHorsePower, 0, 600, 0, HEIGHT-13);

  
  u8g2.firstPage();
  do {
    drawAxisY();
    drawAxisX();
    drawLine();
  } while(u8g2.nextPage() );
}

void resetVirtualDyno(void){
  delay(2000);
  resetTime = millis();
  // Display summary for 5 seconds
  while(millis() - resetTime < 5000){
    
    u8g2.setFont(u8g2_font_freedoomr10_mu);

    u8g2.firstPage();
    do {

      u8g2.drawStr(0,20, "MAX HP: ");
      u8g2.setCursor(60, 20);
      u8g2.print(maxHorsePower);
      u8g2.drawStr(0,40, "MAX TQ: ");
      u8g2.setCursor(60, 40);
      u8g2.print(maxTorque);

    } while(u8g2.nextPage() );
  }
  setupGraph();
}
