#include <Arduino.h>
#include "globals.h"
#include "parseCan.h"
#include <U8x8lib.h>
#include "zero_sixty.h"
#include "canInit.h"

U8X8_SSD1306_128X64_NONAME_4W_SW_SPI u8x8(  /* clock/D0=*/ 13, 
                                            /* data/D1=*/ 14, 
                                            /* cs=*/ 17, 
                                            /* dc=*/ 16, 
                                            /* reset=*/ 15);                                  

volatile int lastSpeed = -1;
volatile uint32_t lastSpeedTime = 0;
volatile uint32_t distance = 0;
volatile uint32_t startTime = 0;
volatile uint32_t dataTime = 0;
 
uint32_t reset_delay = 0;

#define STAGE_IDLE 0
#define STAGE_WAIT_START 1
#define STAGE_MEASURING 2
 
#define FONT_SIZE_SMALL u8x8_font_5x7_f
#define FONT_SIZE_MEDIUM u8x8_font_7x14B_1x2_f
#define FONT_SIZE_XLARGE u8x8_font_profont29_2x3_n

byte stage = STAGE_IDLE;
 
#define SPEED_THRESHOLD_1 60 /* mph */
#define SPEED_THRESHOLD_2 100 /* mph */
#define SPEED_THRESHOLD_3 120 /* mph */
#define DISTANCE_THRESHOLD 1320 /* feet */
 
int times[4] = {0};
 
 
void adjustBrightnessU8X8(){
    if (dimmer == 0) u8x8.setContrast(0);
    else if (dimmer == 1) u8x8.setContrast(0);
    else if (dimmer == 2) u8x8.setContrast(0);
    else if (dimmer == 3) u8x8.setContrast(0);
    else if (dimmer == 4) u8x8.setContrast(0);
    else if (dimmer == 5) u8x8.setContrast(50);
    else if (dimmer == 6) u8x8.setContrast(100);
    else if (dimmer == 7) u8x8.setContrast(175);
    else if (dimmer == 8) u8x8.setContrast(220);
    else if (dimmer == 9) u8x8.setContrast(255);
    else if (dimmer == 10) u8x8.setContrast(255);
}

void setupZeroSixty(){
    u8x8.begin();
    initTimerScreen();
}

void drawZeroSixty(){
    if (setupInitScreen == true){
        u8g2.clear();
        setupZeroSixty();
        setupInitScreen = false;
    }
    adjustBrightnessU8X8();
    timerLoop(); 
}

void timerLoop(){
    unsigned long elapsed = millis() - startTime;
    dataTime = millis();
    u8x8.setFont(FONT_SIZE_XLARGE);

    Serial.print("speed: ");
    Serial.println(speed);    
    Serial.print("lastspd: ");
    Serial.println(lastSpeed);    
    Serial.print("datatime: ");
    Serial.println(dataTime);    
    Serial.print("lastspeedtime: ");
    Serial.println(lastSpeedTime);

    // estimate distance
    //distance is speed X time
    // first we get distance in miles. Then we convert to feet.
    unsigned long calc_dist = ((speed) * (elapsed/1000))*5280;
    distance += calc_dist;


    Serial.print("Distance: ");
    Serial.println(distance);
    Serial.print("CALC Distance: ");
    Serial.println(calc_dist);

    if (lastSpeed != speed) {
        u8x8.setCursor(0,3);
        //make sure we don't overflow the draw buffer
        u8x8.print(speed);
        lastSpeed = speed;
        u8x8.setFont(FONT_SIZE_SMALL);
        u8x8.setCursor(2, 6);
        u8x8.print("mph");
    }
    lastSpeedTime = dataTime;


    if (stage == STAGE_WAIT_START) {
        if (speed == 0) {
            stage = STAGE_MEASURING;
            startTime = lastSpeedTime;
            lastSpeed = 0;
            distance = 0;
            memset(times, 0, sizeof(times));
            initTimerScreen();
        }
    }
    else if (stage == STAGE_MEASURING) {
        //initTimerScreen();
        if (speed != 0) {
        drawTime();
        }
        if (times[2] == 0 && speed >= SPEED_THRESHOLD_3) {
            times[2] = (int)(elapsed / 100);
            //stage = STAGE_IDLE;
            //u8x8.clear();
            showTimerResults();
        } 
        else if (times[1] == 0 && speed >= SPEED_THRESHOLD_2) {
            times[1] = (int)(elapsed / 100);
            showTimerResults();
        } 
        else if (times[0] == 0  && speed >= SPEED_THRESHOLD_1) {
            times[0] = (int)(elapsed / 100);
            showTimerResults();
        } 
        else if (speed == 0) {
            // speed go back to 0
            stage = STAGE_IDLE;
        }
        if (distance > 0 && stage == STAGE_MEASURING) {

            if (distance >= DISTANCE_THRESHOLD) {
                u8x8.setFont(FONT_SIZE_MEDIUM);
                times[3] = (int)(elapsed / 100);
                stage = STAGE_IDLE;
                showTimerResults();
            } 
            else {
                u8x8.setFont(FONT_SIZE_SMALL);
                u8x8.setCursor(12, 6);
                u8x8.print((int)distance);
            }
        }
    }
    else {
        if (speed == 0) {
        stage = STAGE_WAIT_START;
        initTimerScreen();
        u8x8.setFont(FONT_SIZE_SMALL);
        u8x8.setCursor(0.5, 0);
        u8x8.print(" GET");
        u8x8.setCursor(0, 1);
        u8x8.print("READY");
        }
    }
}

void showTimerResults(){
    Serial.println("Showing Timer Results!");
    u8x8.setFont(FONT_SIZE_SMALL);
    u8x8.drawString(5.5,0," 0~60 :");
    u8x8.drawString(5.5,2," 0~100:");
    u8x8.drawString(5.5,4," 0~120:");
    u8x8.drawString(5.5,6," 1/4mi:");
    u8x8.setFont(FONT_SIZE_MEDIUM);


    char buf[12];
    if (times[0]) {
        sprintf(buf, "%2d.%1d", times[0] / 10, times[0] % 10);
        u8x8.setCursor(11.5, 0);
        u8x8.print(buf);
    }
    if (times[1]) {
        sprintf(buf, "%2d.%1d", times[1] / 10, times[1] % 10);
        u8x8.setCursor(11.5, 2);
        u8x8.print(buf);
    }
    if (times[2]) {
        sprintf(buf, "%2d.%1d", times[2] / 10, times[2] % 10);
        u8x8.setCursor(11.5, 4);
        u8x8.print(buf);
    }
    if (times[3]) {
        sprintf(buf, "%2d.%1d", times[3] / 10, times[3] % 10);
        u8x8.setCursor(11.5, 6);
        u8x8.print(buf);
    }
}

void initTimerScreen(){
        Serial.println("INIT SCREEN!");
        //clear the screen
        if (millis() - reset_delay > 1000) {
        reset_delay = millis();
        u8x8.clear();
        }
        //redraw the results
        showTimerResults();
        u8x8.setFont(FONT_SIZE_XLARGE);
        u8x8.setCursor(0,3);
        u8x8.print("0");
        u8x8.setFont(FONT_SIZE_SMALL);
        u8x8.setCursor(2, 6);
        u8x8.print("mph");
}

void drawTime(){
    //draw current time in bottom right corner
    char buf[12];
    unsigned int t = (millis() - startTime);
    sprintf(buf, "%02u:%02u", t / 1000 %60, (int)floor(t/100));
    u8x8.setFont(FONT_SIZE_MEDIUM);
    u8x8.setCursor(0,0);
    u8x8.print(buf);
}