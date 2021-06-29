#include "globals.h"
#include "init_screen.h"
#include "boost_gauge.h"
#include <U8g2lib.h>

int boostPressure;
int boostMax = 0;
int boostMin = 0;

const int sensorHistoryLength = 128;
int sensorHistory[sensorHistoryLength];
int sensorHistoryPos = sensorHistoryLength - 1;

const int boostPin = 20;

unsigned long startMillis = 0;
unsigned long currentMillis = 0;
const unsigned long period = 50;
unsigned long maxtimer = 0;

void drawBoostGauge(){
  if (setupInitScreen == true){
    initScreen();
    setupInitScreen = false;
  }

  // Only read from the sensors every 50 ms
  currentMillis = millis();
  if (currentMillis - startMillis >= period) {
    readSensorData();
    startMillis = currentMillis;
  }
  if (millis() - maxtimer > 10000){
    boostMax = 0;
    maxtimer = 0;
  }

  u8g2.firstPage();
  do {
    // Draw current pressure
    u8g2.setFont(u8g2_font_fub20_tf);
    char cstr[6];
    dtostrf((float)boostPressure / 100, 1, 2, cstr);
    u8g2.drawStr(0, 20, cstr);

    // Draw max pressure
    u8g2.setFont(u8g2_font_fub11_tf);
    dtostrf((float)boostMax / 100, 1, 2, cstr);
    int yPos = u8g2.getStrWidth(cstr);
    u8g2.drawStr(128 - yPos, 11, cstr);

    drawBarGraph(0, 22, 128, 8);
    drawGraph(0, 32, 128, 31);

    u8g2.setFont(u8g2_font_fub11_tf);
    u8g2.setFontMode(1);
    u8g2.setDrawColor(0);
    u8g2.drawStr(10,31, "BOOST");
    u8g2.setFontMode(0);
    u8g2.setDrawColor(1);

  } while ( u8g2.nextPage() );
}


float normaliseSensorData(int boostnumberraw) {
  /*
    Scale the sensor reading into range
    m = measurement to be scaled
    rmin = minimum of the range of the measurement
    rmax = maximum of the range of the measurement
    tmin = minimum of the range of the desired target scaling
    tmax = maximum of the range of the desired target scaling
    normalisedValue = ((m − rmin) / (rmax − rmin)) * (tmax − tmin) + tmin
    https://stats.stackexchange.com/a/281164
  */

  /*
    Sensor voltage ranges from 0.5v to 4.5v, converted to analogRead values (0 min, 1023 max) that's 102 to 921
    rmin = 102
    rmax = 921
    Sensor reads from 0 to 50psi
    tmin = 0
    tmax = 3000
    
    normalisedValue = ((m − 102) / (921 − 102)) * (3000 − 0) + 0
    normalisedValue = ((m − 102) / 819) * 3000
    normalisedValue = (m − 102) / 0.273

  */
  Serial.print("Boost RAW: ");
  Serial.println(boostnumberraw);

  //return ((boostnumberraw - 102) / 0.1356);
  //return ((boostnumberraw - 150) / ((1600 - 150) / (6000)));
  return ((boostnumberraw - 160) / .241);
}


void readSensorData(void) {
  float absolutePressure = normaliseSensorData(analogRead(boostPin));
  
  // Subtract 14.7 psi == pressure at sea level
  // Additional 2.57psi subtracted as boost was showing 2.57 with engine off
  //boostPressure = absolutePressure - 1470;
  boostPressure = absolutePressure;

  Serial.print("Boost Pressure: ");
  Serial.println(absolutePressure);
  //Serial.println(normaliseSensorData(analogRead(boostPin)));

  // Update max and min
  if (boostPressure > boostMax) {
      boostMax = boostPressure;
      maxtimer = millis();
  }

  if (boostPressure < boostMin) boostMin = boostPressure;

  // Log the history
  addSensorHistory(boostPressure);
}


void addSensorHistory(int val) {
  sensorHistory[sensorHistoryPos] = val;
  sensorHistoryPos--;
  if (sensorHistoryPos < 0) sensorHistoryPos = sensorHistoryLength - 1;
}


int getSensorHistory(int index) {
  index += sensorHistoryPos;
  if (index >= sensorHistoryLength) index = index - sensorHistoryLength;
  return sensorHistory[index];
}


// Display functions
void drawGraph(int x, int y, int len, int height) {
  // Draw the lines
  drawHorizontalDottedLine(x, y, len);
  drawHorizontalDottedLine(x, y + height, len);

  //var absMin = Math.abs(boostMin);
  int absMin = abs(boostMin);
  int range = absMin + boostMax;

  // Draw 0 line
  int zeroYPos = mapValueToYPos(absMin, range, y, height);
  drawHorizontalDottedLine(x, zeroYPos, len);

  // Draw the graph line
  for (int i = 0; i < 128; i++) {
    // Scale the values so that the min is always 0
    int valueY = getSensorHistory(i) + absMin;

    // Calculate the coordinants
    int yPos = mapValueToYPos(valueY, range, y, height);
    int xPos = len - i;
    if (yPos < zeroYPos) {
      // Point is above zero line, fill in space under graph
      u8g2.drawVLine(xPos, yPos, zeroYPos + 1 - yPos);
    } else {
      // Point is below zero line, draw graph line without filling in
      u8g2.drawPixel(xPos, yPos);
    }
  }

}

void drawBarGraph(int x, int y, int len, int height) {
  if (boostPressure > 0) {
    // Draw the pressure bar behind the graph
    int barLength = ((float)boostPressure / boostMax) * len;
    u8g2.setDrawColor(2);
    u8g2.drawBox(x, y, barLength, height);
    u8g2.setDrawColor(1);
  }
}

// Maps a value to a y height
int mapValueToYPos(int val, int range, int y, int height) {
  float valueY = ((float)val / range) * height;
  return y + height - (int)valueY;
}

void drawHorizontalDottedLine(int x, int y, int len) {
  for (int i = 0; i < len; i++) {
    if (!(i % 4)) u8g2.drawPixel(x + i, y);
  }
}
