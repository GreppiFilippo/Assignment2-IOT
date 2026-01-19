#ifndef __HW_PLATFORM__
#define __HW_PLATFORM__

#include "config.hpp"
#include "devices/Button.hpp"
#include "devices/Led.hpp"
#include "devices/ServoMotor.hpp"
#include "devices/TempSensor.hpp"

/**
 * @brief Class representing the hardware platform abstraction.
 *
 */
class HWPlatform
{
   private:
    Button* pButton;
    Led* pLed;
    ServoMotor* pMotor;
    TempSensor* pTempSensor;

   public:
    HWPlatform();
    void init();
    void test();

    Button* getButton();
    Led* getLed();
    ServoMotor* getMotor();
    TempSensor* getTempSensor();
};

#endif
