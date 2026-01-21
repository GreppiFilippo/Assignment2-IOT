#include "HWPlatform.hpp"

#include <Arduino.h>

#include "config.hpp"
#include "devices/ButtonImpl.hpp"
#include "devices/Led.hpp"
#include "devices/LightSensorImpl.hpp"
#include "devices/Pir.hpp"
#include "devices/ServoMotorImpl.hpp"
#include "devices/Sonar.hpp"
#include "devices/TempSensorTMP36.hpp"
#include "kernel/Logger.hpp"
#include "kernel/MsgService.hpp"


void wakeUp() {}

HWPlatform::HWPlatform()
{
    button = new ButtonImpl(RESET_PIN);
    l1 = new Led(L1_PIN);
    l2 = new Led(L2_PIN);
    l3 = new Led(L3_PIN);
    presenceSensor = new Pir(DPD_PIN);
    lcd = new LCD(LCD_ADR, LCD_COL, LCD_ROW);
    motor = new ServoMotorImpl(HD_PIN);
    tempSensor = new TempSensorTMP36(TEMP_PIN);
    proximitySensor = new Sonar(DDD_PIN_E, DDD_PIN_T, 100);
}

void HWPlatform::init() {}

Button* HWPlatform::getButton() { return this->button; }

Light* HWPlatform::getL1() { return this->l1; }

Light* HWPlatform::getL2() { return this->l2; }

Light* HWPlatform::getL3() { return this->l3; }

ServoMotor* HWPlatform::getMotor() { return this->motor; }

TempSensor* HWPlatform::getTempSensor() { return this->tempSensor; }

PresenceSensor* HWPlatform::getPresenceSensor() { return this->presenceSensor; }

LCD* HWPlatform::getLCD() { return this->lcd; }

ProximitySensor* HWPlatform::getProximitySensor() { return this->proximitySensor; }

void HWPlatform::test()
{
    // TODO: implement hardware test
}
