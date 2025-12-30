#ifndef __LED_EXT__
#define __LED_EXT__

#include "Led.hpp"
#include "LightExt.hpp"

/**
 * @brief Extended LED class with intensity control.
 *
 */
class LedExt : public LightExt, public Led
{
   public:
    LedExt(int pin);
    LedExt(int pin, int intensity);
    void switchOn() override;
    void switchOff() override;
    void setIntensity(int v) override;

   private:
    int currentIntensity;
    bool isOn;
};

#endif
