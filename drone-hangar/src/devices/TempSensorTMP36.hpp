#ifndef __TEMP_SENSOR_TMP36__
#define __TEMP_SENSOR_TMP36__

#include "TempSensor.hpp"

/**
 * @brief Class representing a TMP36 temperature sensor.
 *
 */
class TempSensorTMP36 : public TempSensor
{
   public:
    TempSensorTMP36(int p);
    float getTemperature() override;

   private:
    int pin;
};

#endif
