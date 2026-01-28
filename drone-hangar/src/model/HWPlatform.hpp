#ifndef __HW_PLATFORM__
#define __HW_PLATFORM__

#include "config.hpp"
#include "devices/Button.hpp"
#include "devices/LCD.hpp"
#include "devices/Led.hpp"
#include "devices/PresenceSensor.hpp"
#include "devices/ProximitySensor.hpp"
#include "devices/ServoMotor.hpp"
#include "devices/TempSensor.hpp"

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
    /**
     * @brief Construct a new HWPlatform object
     *
     */
    HWPlatform();

    /**
     * @brief Initialize the hardware platform.
     *
     */
    void init();

    /**
     * @brief Run hardware tests.
     *
     */
    void test();

    /**
     * @brief Get the Button object
     *
     * @return Button* pointer to the Button
     */
    Button* getButton();

    /**
     * @brief Get the L1 Light object
     *
     * @return Light* pointer to the L1 Light
     */
    Light* getL1();

    /**
     * @brief Get the L2 Light object
     *
     * @return Light* pointer to the L2 Light
     */
    Light* getL2();

    /**
     * @brief Get the L3 Light object
     *
     * @return Light* pointer to the L3 Light
     */
    Light* getL3();

    /**
     * @brief Get the Motor object
     *
     * @return ServoMotor* pointer to the Servo Motor
     */
    ServoMotor* getMotor();

    /**
     * @brief Get the Temp Sensor object
     *
     * @return TempSensor* pointer to the Temperature Sensor
     */
    TempSensor* getTempSensor();

    /**
     * @brief Get the Presence Sensor object
     *
     * @return PresenceSensor*
     */
    PresenceSensor* getPresenceSensor();

    /**
     * @brief Get the LCD object
     *
     * @return LCD* pointer to the LCD
     */
    LCD* getLCD();

    /**
     * @brief Get the Proximity Sensor object
     *
     * @return ProximitySensor* pointer to the Proximity Sensor
     */
    ProximitySensor* getProximitySensor();
};

#endif
