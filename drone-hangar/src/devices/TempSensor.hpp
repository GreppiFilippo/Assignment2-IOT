#ifndef __TEMP_SENSOR__
#define __TEMP_SENSOR__

/**
 * @brief Abstract base class for temperature sensors.
 *
 */
class TempSensor
{
   public:
    /**
     * Get the temperature measured by the sensor.
     *
     * @return float representing the temperature in appropriate units
     */
    virtual float getTemperature() = 0;
};

#endif
