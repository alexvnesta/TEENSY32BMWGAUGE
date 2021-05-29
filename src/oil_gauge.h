#ifndef OIL_GAUGE_H
#define OIL_GAUGE_H

void drawOilGauge(void);
void readOilSensorData(void);
void drawOilGraph(int x, int y, int len, int height);
void drawOilBarGraph(int x, int y, int len, int height);
float normaliseOilSensorData(int m);
void addOilSensorHistory(int val);
int getOilSensorHistory(int index);
extern int oilPressure;
#endif
