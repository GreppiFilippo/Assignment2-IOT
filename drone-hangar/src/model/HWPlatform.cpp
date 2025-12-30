#include "HWPlatform.hpp"

#include <Arduino.h>

#include "config.hpp"
#include "devices/ButtonImpl.hpp"
#include "devices/Led.hpp"
#include "devices/LightSensorImpl.hpp"
#include "devices/ServoMotorImpl.hpp"
#include "devices/TempSensorTMP36.hpp"
#include "kernel/Logger.hpp"
#include "kernel/MsgService.hpp"

void wakeUp() {}

HWPlatform::HWPlatform()
{
    pButton = new ButtonImpl(BT_PIN);
    pLed = new Led(LED_PIN);
    pMotor = new ServoMotorImpl(MOTOR_PIN);
    pTempSensor = new TempSensorTMP36(TEMP_SENSOR_PIN);
}

void HWPlatform::init() {}

Button* HWPlatform::getButton() { return this->pButton; }

Led* HWPlatform::getLed() { return this->pLed; }

ServoMotor* HWPlatform::getMotor() { return this->pMotor; }

TempSensor* HWPlatform::getTempSensor() { return this->pTempSensor; }

void HWPlatform::test()
{
    bool btPressed = pButton->isPressed();
    pLed->switchOn();
    pMotor->on();
    pMotor->setPosition(90);
    Logger.log("Button: " + String(btPressed ? "pressed" : " not pressed"));
    delay(1000);
    pMotor->setPosition(0);
    delay(1000);
    pMotor->off();
    pLed->switchOff();
}
