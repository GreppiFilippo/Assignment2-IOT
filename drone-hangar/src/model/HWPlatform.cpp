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

#define MAX_TIME 30000

void wakeUp() {}

HWPlatform::HWPlatform()
{
    this->button = new ButtonImpl(RESET_PIN);
    this->l1 = new Led(L1_PIN);
    this->l2 = new Led(L2_PIN);
    this->l3 = new Led(L3_PIN);
    this->presenceSensor = new Pir(DPD_PIN);
    this->lcd = new LCD(LCD_ADR, LCD_COL, LCD_ROW);
    this->motor = new ServoMotorImpl(HD_PIN);
    this->tempSensor = new TempSensorTMP36(TEMP_PIN);
    this->proximitySensor = new Sonar(DDD_PIN_E, DDD_PIN_T, MAX_TIME);
}

void HWPlatform::init() { motor->on(); }

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
    static int step = 0;
    static int subStep = 0;
    static unsigned long lastStepTime = 0;
    unsigned long now = millis();

    if (lastStepTime == 0)
        lastStepTime = now;

    switch (step)
    {
        case 0:
            Logger.log(F("=== HW TEST START ==="));
            lcd->clear();
            lcd->print("HW TEST START");

            l1->switchOff();
            l2->switchOff();
            l3->switchOff();
            motor->off();

            step++;
            lastStepTime = now;
            break;

        case 1:
            if (now - lastStepTime > 300)
            {
                subStep++;
                if (subStep == 1)
                {
                    l1->switchOn();
                    Logger.log(F("[TEST] L1 ON"));
                }
                else if (subStep == 2)
                {
                    l1->switchOff();
                    l2->switchOn();
                    Logger.log(F("[TEST] L2 ON"));
                }
                else if (subStep == 3)
                {
                    l2->switchOff();
                    l3->switchOn();
                    Logger.log(F("[TEST] L3 ON"));
                }
                else if (subStep == 4)
                {
                    l3->switchOff();
                    Logger.log(F("[TEST] LEDs OFF"));
                    step++;
                    subStep = 0;
                }
                lastStepTime = now;
            }
            break;

        case 2:
            if (subStep == 0)
            {
                Logger.log(F("[TEST] Servo -> OPEN (180)"));
                motor->on();
                motor->setPosition(180);
                subStep++;
                lastStepTime = now;
            }
            else if (subStep == 1 && (now - lastStepTime > 1500))
            {
                Logger.log(F("[TEST] Servo -> CLOSE (0)"));
                motor->setPosition(0);
                subStep++;
                lastStepTime = now;
            }
            else if (subStep == 2 && (now - lastStepTime > 1500))
            {
                motor->off();
                step++;
                subStep = 0;
                lastStepTime = now;
            }
            break;

        case 3:
        {
            if (subStep == 0)
            {
                Logger.log(F("=== SENSOR MONITOR (10s) ==="));
                lcd->clear();
                lcd->print("SENSORS TEST...");
                subStep = 1;
                lastStepTime = now;
            }

            if (now - lastStepTime > 10000)
            {
                step++;
                subStep = 0;
                break;
            }

            float temp = tempSensor->getTemperature();
            float dist = proximitySensor->getDistance();
            bool pir = presenceSensor->isDetected();
            bool btn = button->isPressed();

            String logMsg = "SENS | T:" + String(temp, 1) + "C | D:" + String(dist, 0) + "cm";
            logMsg += " | PIR:" + String(pir ? "YES" : "NO");
            logMsg += " | BTN:" + String(btn ? "ON" : "OFF");
            Logger.log(logMsg);

            static unsigned long lastLcdUpdate = 0;
            if (now - lastLcdUpdate > 1000)
            {
                String lcdMsg = "T:" + String((int)temp) + " D:" + String((int)dist) +
                                " P:" + String(pir) + " B:" + String(btn);
                lcd->print(lcdMsg.c_str());
                lastLcdUpdate = now;
            }
            break;
        }

        case 4:
            Logger.log(F("=== TEST COMPLETE ==="));
            lcd->clear();
            lcd->print("TEST DONE");
            step = 0;
            subStep = 0;
            lastStepTime = now;
            break;
    }
}
