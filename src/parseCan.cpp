#include <Arduino.h>
#include "parseCan.h"
#include "canInit.h"
#include "globals.h"
#include "cartesian_graph.h"
#include "init_screen.h"
#include "zero_sixty.h"
#include <Snooze.h>


//static uint32_t RADIO_ID = 1250;
static uint32_t STEERING_WHEEL_ID = 470; //1D6 HEX
static uint32_t BATTERY_ID = 948; //3B4 HEX
//static uint32_t SPEED_ID = 422; //1A6 HEX
static uint32_t SPEED_ID = 436; //1B4 HEX
static uint32_t TORQUE_ID = 168; //0A8
static uint32_t RPM_ID = 170; //0AA
static uint32_t COOLANT_ID = 464; //1D0
static uint32_t DASHLIGHT_ID = 514; //202

volatile float voltage = 0;
volatile int canSpeed = 0;
volatile float torquenm = 0;
volatile float torquelbf = 0;
volatile int rpm = 0; 
volatile float coolantC = 0;
volatile float coolantF = 0;
volatile int dimmer = 0; 

unsigned volatile long lastButtonPress = 0;
int volatile clickCounter;

unsigned volatile long lastButtonPressTime = 0;

unsigned long snoozeTime = 0;
bool snoozePin = HIGH;

int volatile numClicks = 0;

SnoozeDigital digital;
SnoozeBlock config_teensy32(digital);

bool sleepflag = 0;
void checkSnooze(void){
  pinMode(21, INPUT_PULLDOWN);

  Serial.print("Checking PIN21 state: ");

  //digital.pinMode(21, INPUT_DISABLE, RISING);
  digital.pinMode(21, INPUT_PULLDOWN, RISING);
  snoozePin = digitalRead(21);
  Serial.println(snoozePin);

  if (snoozePin == LOW){
      Serial.println("SLEEP PIN IS LOW");

      if (millis() - snoozeTime > 2000) {
        Serial.println("GOING TO SLEEP NOW!!");
        global_screen_value = 2;
        sleepflag = 1;
        u8g2.setPowerSave(1);
        Snooze.deepSleep( config_teensy32 ); 
      }
  }
  else if(snoozePin == HIGH) {
    snoozeTime = millis();
    // only reinitialize after sleep.
    if (sleepflag == 1){
          //initCanT4();
          setup();
          //u8g2.setPowerSave(0);
          sleepflag = 0;
    }

    //Serial.println("PIN IS HIGH");

  }
}

void parseCanMessage(uint32_t id, const uint8_t message[], uint8_t messageLength){
    
    if (id == STEERING_WHEEL_ID){
    Serial.println("STEERING WHEEL BUTTON!!!!!");
   
        if (message[0] == 192 && message[1] == 13 && (millis() - lastButtonPressTime > 150)){
          lastButtonPressTime = millis();
          Serial.println("MATCHES VOICE BUTTON!");
          numClicks = checkNumClicks();
          setupInitScreen = true;

            /*
            if(numClicks == 2 && global_screen_value == 4){
              //Double click event and on virtual dyno page
              Serial.println("RESET VIRTUAL DYNO!");
              resetVirtualDyno();

            }
            else if(numClicks == 2 && global_screen_value == 5){
              //Double click event and on zero sixty page
              Serial.println("RESET ZERO SIXTY!");
              setupZeroSixty();

            }
            */
            if(numClicks == 2){
              Serial.println("DOUBLE ClICK!");
            }
            else if(numClicks == 1){
              //Single click event
              Serial.println("SINGLE ClICK!");
              lastButtonPress = millis();
              if (global_screen_value < 7) global_screen_value += 1;
              if (global_screen_value == 7) global_screen_value = 0;
              switchScreens(global_screen_value);
            }
          
        }
  }

  else if (id == BATTERY_ID){
    voltage = (((message[1]-240 )*256)+message[0])/68;
    Serial.print("Voltage: ");
    Serial.println(voltage);
  }
  else if (id == SPEED_ID){
    canSpeed = (((message[1]-208)*256)+message[0])/16;
    Serial.print("Speed: ");
    Serial.println(canSpeed);
  }
  else if (id == TORQUE_ID){
    torquenm = ((message[2]*256)+message[1]) / 32;
    torquelbf = (torquenm*0.73756214728);
    if (torquelbf > 800) torquelbf = 0;
    Serial.print("Torque: ");
    Serial.println(torquenm);
  }
  else if (id == RPM_ID){
    rpm = ((message[5] * 256) + message[4] ) / 4;
    Serial.print("RPM: ");
    Serial.println(rpm);
  }
  else if (id == COOLANT_ID){
    coolantC = message[0] - 48;
    coolantF = (coolantC*1.8)+32;
    Serial.print("Coolant C: ");
    Serial.println(coolantC);
  }
  else if (id == DASHLIGHT_ID){
    /*
    Dimmer value = 0, Byte 0 = 0
    Dimmer value = 1, Byte 0 = 28
    Dimmer value = 2, Byte 0 = 56
    Dimmer value = 3, Byte 0 = 84
    Dimmer value = 4, Byte 0 = 112
    Dimmer value = 5, Byte 0 = 141
    Dimmer value = 6, Byte 0 = 169
    Dimmer value = 7, Byte 0 = 197
    Dimmer value = 8, Byte 0 = 225
    Dimmer value = 9, Byte 0 = 253

    When the lights are off, Byte 0 = 254
    */

    volatile int dimmermsg = message[0];
    if (dimmermsg == 0) dimmer = 0;
    else if (dimmermsg == 28) dimmer = 1;
    else if (dimmermsg == 56) dimmer = 2;
    else if (dimmermsg == 84) dimmer = 3;
    else if (dimmermsg == 112) dimmer = 4;
    else if (dimmermsg == 141) dimmer = 5;
    else if (dimmermsg == 169) dimmer = 6;
    else if (dimmermsg == 197) dimmer = 7;
    else if (dimmermsg == 225) dimmer = 8;
    else if (dimmermsg == 253) dimmer = 9;
    else if (dimmermsg == 254) dimmer = 10;
    Serial.print("Dimmer: ");
    Serial.println(dimmer);
  }
}

int checkNumClicks(void){
    static unsigned long max_delay = 800;
    int returnVal = 0;
    Serial.print("before Adding to Counter");
    Serial.println(clickCounter);
    clickCounter += 1;
    Serial.print("After Adding to Counter");
    Serial.println(clickCounter);

    lastButtonPress = millis();

    while((millis() - lastButtonPress < max_delay) && clickCounter == 1){
        can1.events();
        if (clickCounter == 2){
        //Serial.println("Button pressed twice!");
        returnVal = 2;
        clickCounter = 0;
        lastButtonPress = millis();
        break;
        return(returnVal);
        }
    }
    if (clickCounter == 1){
        //Serial.println("Button pressed once!");
        clickCounter = 0;
        returnVal = 1;
        lastButtonPress = millis();
        return(returnVal);
    }

    return (returnVal);
}
