#ifndef __TEMP_SENSOR_TMP36__
#define __TEMP_SENSOR_TMP36__

#include "TempSensor.hpp"

class TempSensorTMP36: public TempSensor {
public:
  TempSensorTMP36(int p);
  virtual float getTemperature();
private:
  int pin;
};


#endif
