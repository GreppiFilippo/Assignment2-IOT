#include "HWPlatform.hpp"

#include <Arduino.h>

#include "config.hpp"
#include "devices/ButtonImpl.hpp"
#include "devices/LCD.hpp"
#include "devices/Led.hpp"
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

void HWPlatform::init() {
    motor->on();
}

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
    static int testStep = 0;

    Logger.log("=== HW TEST STEP " + String(testStep) + " ===");

    switch (testStep)
    {
        case 0:
            Logger.log("Testing LEDs...");
            l1->switchOn();
            l2->switchOn();
            l3->switchOn();
            break;

        case 1:
            Logger.log("Testing L1 OFF");
            l1->switchOff();
            break;

        case 2:
            Logger.log("Testing L2 OFF");
            l2->switchOff();
            break;

        case 3:
            Logger.log("Testing L3 OFF");
            l3->switchOff();
            break;

        case 4:
            Logger.log("Testing LCD...");
            lcd->clear();
            lcd->print("HW TEST OK");
            break;

        case 5:
            Logger.log("Testing LCD line 2");
            lcd->clear();
            lcd->print("Line 1 Line 2 Line 3 Line 4");
            break;

        case 6:
            Logger.log("Testing Servo - Position 0");
            motor->on();
            motor->setPosition(0);
            break;

        case 7:
            Logger.log("Testing Servo - Position 90");
            motor->setPosition(90);
            break;

        case 8:
            Logger.log("Testing Servo - Position 180");
            motor->setPosition(180);
            break;

        case 9:
            Logger.log("Testing Servo OFF");
            motor->off();
            break;

        case 10:
        {
            Logger.log("Testing Temp Sensor...");
            int temp = tempSensor->getTemperature();
            Logger.log("Temperature: " + String(temp) + " C");
            break;
        }

        case 11:
        {
            Logger.log("Testing Sonar...");
            float distance = proximitySensor->getDistance();
            Logger.log("Distance: " + String(distance) + " cm");
            break;
        }

        case 12:
        {
            Logger.log("Testing PIR...");
            bool presence = presenceSensor->isDetected();
            Logger.log("Presence detected: " + String(presence ? "YES" : "NO"));
            break;
        }

        case 13:
        {
            Logger.log("Testing Button...");
            bool pressed = button->isPressed();
            Logger.log("Button pressed: " + String(pressed ? "YES" : "NO"));
            break;
        }

        case 14:
            Logger.log("=== TEST COMPLETE - RESTARTING ===");
            lcd->clear();
            lcd->print("TEST COMPLETE");
            l1->switchOn();
            l2->switchOn();
            l3->switchOn();
            break;

        case 15:
            l1->switchOff();
            l2->switchOff();
            l3->switchOff();
            testStep = -1;  // Will become 0 after increment
            break;
    }

    testStep++;
}
