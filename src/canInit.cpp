#include <Arduino.h>
#include "canInit.h"
#include "parseCan.h"


//static uint32_t STEERING_WHEEL_ID = 470; //1D6 HEX
//static uint32_t BATTERY_ID = 948; //3B4 HEX
//static uint32_t SPEED_ID = 436; //1B4 HEX
//static uint32_t TORQUE_ID = 168; //0A8
//static uint32_t RPM_ID = 170; //0AA
//static uint32_t COOLANT_ID = 464; //1D0

FlexCAN_T4<CAN0, RX_SIZE_256, TX_SIZE_16> can1;

CAN_message_t canMsg;

void initCanT4(void){

  can1.begin();
  can1.setBaudRate(100E3);
  can1.setMaxMB(8);

  can1.setMB(MB0, RX, STD);
  can1.setMB(MB1, RX);
  can1.setMB(MB2, RX);
  can1.setMB(MB3, RX);
  can1.setMB(MB4, RX);
  can1.setMB(MB5, RX);
  can1.setMB(MB6, RX);


  can1.setMBFilter(REJECT_ALL);
  can1.enableMBInterrupt(MB0);

  can1.onReceive(MB0,canSniff);
  can1.onReceive(MB1,canSniff);
  can1.onReceive(MB2,canSniff);
  can1.onReceive(MB3,canSniff);
  can1.onReceive(MB4,canSniff);
  can1.onReceive(MB5,canSniff);
  can1.onReceive(MB6,canSniff);


  can1.setMBFilter(MB0, 0x1D6);
  can1.setMBFilter(MB1, 0x3B4);
  can1.setMBFilter(MB2, 0x1B4);
  can1.setMBFilter(MB3, 0x0A8);
  can1.setMBFilter(MB4, 0x0AA);
  can1.setMBFilter(MB5, 0x1D0);
  can1.setMBFilter(MB6, 0x202);

  
  can1.distribute();

  //can1.onReceive(canSniff);

//can1.mailboxStatus();

}

void canSniff(const CAN_message_t &msg) {
  
  Serial.print("MB "); Serial.print(msg.mb);
  Serial.print("  OVERRUN: "); Serial.print(msg.flags.overrun);
  Serial.print("  LEN: "); Serial.print(msg.len);
  Serial.print(" EXT: "); Serial.print(msg.flags.extended);
  Serial.print(" TS: "); Serial.print(msg.timestamp);
  Serial.print(" ID: "); Serial.print(msg.id, HEX);
  Serial.print(" Buffer: ");
 
  for ( uint8_t i = 0; i < msg.len; i++ ) {
    Serial.print(msg.buf[i], HEX); Serial.print(" ");
  } Serial.println();
  
  parseCanMessage(msg.id, msg.buf, msg.len);
}

void readCanMessages(){
  if (can1.read(canMsg) ) {
    Serial.print("CAN1 "); 
    Serial.print("MB: "); Serial.print(canMsg.mb);
    Serial.print("  ID: 0x"); Serial.print(canMsg.id, HEX );
    Serial.print("  EXT: "); Serial.print(canMsg.flags.extended );
    Serial.print("  LEN: "); Serial.print(canMsg.len);
    Serial.print(" DATA: ");
    for ( uint8_t i = 0; i < 8; i++ ) {
      Serial.print(canMsg.buf[i]); Serial.print(" ");
    }
    Serial.print("  TS: "); Serial.println(canMsg.timestamp);

    parseCanMessage(canMsg.id, canMsg.buf, canMsg.len);
  }



}
