#ifndef __LIGHT_EXT__
#define __LIGHT_EXT__

#include "Light.hpp"

/**
 * @brief Extended Light class with intensity control.
 *
 */
class LightExt : public Light
{
   public:
    /**
     * @brief Set the intensity of the light.
     *
     */
    virtual void setIntensity(int) = 0;
};

#endif
