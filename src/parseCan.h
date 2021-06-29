#ifndef PARSECAN_H
#define PARSECAN_H
#include <FlexCAN_T4.h>

void parseCanMessage(uint32_t id, const uint8_t message[], uint8_t messageLength);
int checkNumClicks(void);

extern volatile float voltage;
extern volatile int canSpeed;
extern volatile float torquenm;
extern volatile float torquelbf;
extern volatile int rpm; 
extern volatile float coolantC;
extern volatile float coolantF;
extern volatile int dimmer;


#endif