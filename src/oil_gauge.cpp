
#include "globals.h"
#include "init_screen.h"
#include "oil_gauge.h"
#include "boost_gauge.h"
#include <U8g2lib.h>

//extern U8G2_SSD1306_128X64_NONAME_1_4W_SW_SPI u8g2;

const int oilPin = 19;

int oilPressure;
int oilMax = 0;
int oilMin = 0;

bool flashTracker = false;

unsigned long startMillisOil = 0;
unsigned long currentMillisOil = 0;
const unsigned long periodOil = 100;

const int oilSensorHistoryLength = 128;
int oilSensorHistory[oilSensorHistoryLength];
int oilSensorHistoryPos = oilSensorHistoryLength - 1;

void drawOilGauge(void) {
  if (setupInitScreen == true){
    initScreen();
    setupInitScreen = false;
  }

  // Only read from the sensors every 50 ms
  currentMillisOil = millis();
  if (currentMillisOil - startMillisOil >= periodOil) {
    readOilSensorData();
    flashTracker = !flashTracker;
    startMillisOil = currentMillisOil;
  }

  u8g2.firstPage();
  do {
    // Draw current pressure
    u8g2.setFont(u8g2_font_fub20_tf);
    char cstr[6];
    dtostrf((float)oilPressure / 100, 1, 1, cstr);
    u8g2.drawStr(0, 20, cstr);

    // Draw max pressure
    u8g2.setFont(u8g2_font_fub11_tf);
    char indicator[4];
    if (oilPressure < 1600) {
      // LOW
      if (flashTracker) strcpy(indicator, "LOW");
      else strcpy(indicator, "");
    } else if (oilPressure < 2500) {
      // IDLE
      strcpy(indicator, "IDLE");
    } else {
      // LOAD
      strcpy(indicator, "LOAD");
    }
    int yPos = u8g2.getStrWidth(indicator);
    u8g2.drawStr(128 - yPos, 11, indicator);

    drawOilBarGraph(0, 22, 128, 8);
    drawOilGraph(0, 32, 128, 31);
    u8g2.setFont(u8g2_font_freedoomr10_mu);
    u8g2.drawStr(0,34, "OIL PRESS");


  } while ( u8g2.nextPage() );
}


float normaliseOilSensorData(int mOil) {
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
    Sensor reads from 0 to 140psi
    tmin = 0
    tmax = 14000
    normalisedValue = ((m − 102) / (921 − 102)) * (14000 − 0) + 0
    normalisedValue = ((m − 102) / 819) * 14000
    normalisedValue = (m − 102) / 0.0585
  */

  //m - 160 / 921-160 * 14000

  Serial.print ("Oil RAW:");
    Serial.println (mOil);
  
  //return ((mOil - 102) / 0.0819);
  //return ((mOil - 150) / ((1500 - 150) / (10000)));
  return ((mOil - 191) / .131);

}


void readOilSensorData(void) {
  volatile float absolutePressure = normaliseOilSensorData(analogRead(oilPin));
  
  // Subtract 14.7 psi == pressure at sea level
  //oilPressure = absolutePressure - 1470;
  oilPressure = absolutePressure;

  Serial.print("Oil Pressure: ");
  Serial.println(oilPressure);

  // Oil pressure should never be negative
  if (oilPressure < 0) oilPressure = 0;

  // Update max and min
  if (oilPressure > oilMax) oilMax = oilPressure;
  if (oilPressure < oilMin) oilMin = oilPressure;

  // Log the history
  addOilSensorHistory(oilPressure);
}


void addOilSensorHistory(int val) {
  oilSensorHistory[oilSensorHistoryPos] = val;
  oilSensorHistoryPos--;
  if (oilSensorHistoryPos < 0) oilSensorHistoryPos = oilSensorHistoryLength - 1;
}


int getOilSensorHistory(int index) {
  index += oilSensorHistoryPos;
  if (index >= oilSensorHistoryLength) index = index - oilSensorHistoryLength;
  return oilSensorHistory[index];
}


// Display functions

void drawOilGraph(int x, int y, int len, int height) {
  // Draw the lines
  drawHorizontalDottedLine(x, y, len);
  drawHorizontalDottedLine(x, y + height, len);

  int absMin = abs(oilMin);
  int range = absMin + oilMax;

  // Draw 0 line
  int zeroYPos = mapValueToYPos(absMin, range, y, height);
  drawHorizontalDottedLine(x, zeroYPos, len);

  // Draw the graph line
  for (int i = 0; i < 128; i++) {
    // Scale the values so that the min is always 0
    int valueY = getOilSensorHistory(i) + absMin;

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

void drawOilBarGraph(int x, int y, int len, int height) {
  if (oilPressure > 0) {
    // Draw the pressure bar behind the graph
    int barLength = ((float)oilPressure / oilMax) * len;
    u8g2.setDrawColor(2);
    u8g2.drawBox(x, y, barLength, height);
    u8g2.setDrawColor(1);
  }
}
