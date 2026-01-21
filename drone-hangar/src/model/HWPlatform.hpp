#ifndef __HW_PLATFORM__
#define __HW_PLATFORM__

#include "config.hpp"
#include "devices/Button.hpp"
#include "devices/LCD.hpp"
#include "devices/Led.hpp"
#include "devices/PresenceSensor.hpp"
#include "devices/ServoMotor.hpp"
#include "devices/TempSensor.hpp"
#include "devices/ProximitySensor.hpp"

/**
 * @brief Class representing the hardware platform abstraction.
 *
 */
class HWPlatform
{
   private:
    Button* button;
    Light* l1;
    Light* l2;
    Light* l3;
    PresenceSensor* presenceSensor;
    ServoMotor* motor;
    TempSensor* tempSensor;
    LCD* lcd;
    ProximitySensor* proximitySensor;

   public:
    HWPlatform();
    void init();
    void test();

    Button* getButton();

    Light* getL1();

    Light* getL2();

    Light* getL3();

    ServoMotor* getMotor();
    TempSensor* getTempSensor();
    PresenceSensor* getPresenceSensor();

    LCD* getLCD();
    ProximitySensor* getProximitySensor();
};

#endif
