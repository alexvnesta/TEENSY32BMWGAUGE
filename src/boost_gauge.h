
#ifndef BOOST_GAUGE_H
#define BOOST_GAUGE_H
void drawBoostGauge();
float normaliseSensorData(int m);
void readSensorData(void);
void addSensorHistory(int val);
int getSensorHistory(int index);
void drawGraph(int x, int y, int len, int height);
void drawBarGraph(int x, int y, int len, int height);
int mapValueToYPos(int val, int range, int y, int height);
void drawHorizontalDottedLine(int x, int y, int len);
#endif