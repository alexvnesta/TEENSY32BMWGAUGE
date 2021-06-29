#include "globals.h"
#include "boost_gauge.h"
#include "oil_gauge.h"
#include "parseCan.h"

unsigned long multigaugeMillis = 0;
unsigned long multigaugeStartMillis = 0;

float boostValue = 0;
float oilValue = 0;

void drawMultiGauge(void){
    if (setupInitScreen == true){
    initScreen();
    setupInitScreen = false;
    }

    multigaugeMillis = millis();

    if (multigaugeMillis - multigaugeStartMillis > 100) {
        boostValue = normaliseSensorData(analogRead(20))/100;
        oilValue = normaliseOilSensorData(analogRead(19))/100;

        Serial.print("boostValue : ");
        Serial.println(boostValue);
        Serial.print("oilValue : ");
        Serial.println(oilValue);

        multigaugeStartMillis = millis();
    }

    u8g2.firstPage();
    do {
      u8g2.setFont(u8g2_font_amstrad_cpc_extended_8f);
      u8g2.drawStr(0,12,"Boost:");
      u8g2.setCursor(80,12);
      u8g2.print(boostValue, 1);
      //u8g2.drawStr(0,12,"TORQ:");
      //u8g2.setCursor(80,12);
      //u8g2.print(torquelbf);
      u8g2.drawStr(0,25,"OilP:");
      u8g2.setCursor(80,25);
      u8g2.print(oilValue, 1);
      //u8g2.drawStr(0,25,"SPED:");
      //u8g2.setCursor(80,25);
      //u8g2.print(speed);
      u8g2.drawStr(0,37,"VLTG:");
      u8g2.setCursor(80,37);
      u8g2.print(voltage);
      //u8g2.drawStr(0,49,"RPM:");
      //u8g2.setCursor(80,49);
      //u8g2.print(rpm);
      u8g2.drawStr(0,49,"TORQ:");
      u8g2.setCursor(80,49);
      u8g2.print(torquelbf);
      u8g2.drawStr(0,61,"CTEMP:");
      u8g2.setCursor(80,61);
      u8g2.print(coolantF);
  } while ( u8g2.nextPage() );


}