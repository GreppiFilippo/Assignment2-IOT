#ifndef __TEMP_SENSOR_LM35__
#define __TEMP_SENSOR_LM35__

#include "TempSensor.hpp"

/**
 * @brief Class representing a LM35 temperature sensor.
 *
 */
class TempSensorLM35 : public TempSensor
{
   public:
    TempSensorLM35(int pin);
    float getTemperature() override;

   private:
    int pin;
};

#endif
