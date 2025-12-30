#include "LightSensor.hpp"

/**
 * @brief Implementation of a light sensor using an analog pin.
 *
 */
class LightSensorImpl : public LightSensor
{
   public:
    LightSensorImpl(int pin);
    double getLightIntensity() override;

   private:
    int pin;
};
