#ifndef __LIGHTSENSOR__
#define __LIGHTSENSOR__

/**
 * @brief Abstract base class for light sensor devices.
 *
 */
class LightSensor
{
   public:
    /**
     * Get the current light intensity measured by the sensor.
     *
     * @return double representing the light intensity
     */
    virtual double getLightIntensity() = 0;
};
#endif
