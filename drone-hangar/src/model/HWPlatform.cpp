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
    char buf[64]; // Buffer temporaneo per i messaggi composti

    // Invece di "..." + String(testStep), usiamo snprintf
    snprintf(buf, sizeof(buf), "=== HW TEST STEP %d ===", testStep);
    Logger.log(buf);

    switch (testStep)
    {
        case 0:
            Logger.log("Testing LEDs...");
            l1->switchOn();
            l2->switchOn();
            l3->switchOn();
            break;

        // ... case 1-9 rimangono invariati perché usano stringhe fisse ...
        case 1: Logger.log("Testing L1 OFF"); l1->switchOff(); break;
        case 2: Logger.log("Testing L2 OFF"); l2->switchOff(); break;
        case 3: Logger.log("Testing L3 OFF"); l3->switchOff(); break;

        case 10:
        {
            int temp = tempSensor->getTemperature();
            snprintf(buf, sizeof(buf), "Temperature: %d C", temp);
            Logger.log(buf);
            break;
        }

        case 11:
        {
            float distance = proximitySensor->getDistance();
            // Nota: su molti Arduino (AVR), snprintf non supporta %f per i float.
            // Usiamo dtostrf se necessario, o convertiamo in intero:
            snprintf(buf, sizeof(buf), "Distance: %d cm", (int)distance);
            Logger.log(buf);
            break;
        }

        case 12:
        {
            bool presence = presenceSensor->isDetected();
            // Uso dell'operatore ternario direttamente nel log
            Logger.log(presence ? "Presence detected: YES" : "Presence detected: NO");
            break;
        }

        case 13:
        {
            bool pressed = button->isPressed();
            Logger.log(pressed ? "Button pressed: YES" : "Button pressed: NO");
            break;
        }

        case 14:
            Logger.log("=== TEST COMPLETE - RESTARTING ===");
            lcd->clear();
            lcd->print("TEST COMPLETE");
            l1->switchOn(); l2->switchOn(); l3->switchOn();
            break;

        case 15:
            l1->switchOff(); l2->switchOff(); l3->switchOff();
            testStep = -1; 
            break;
        
        default:
            break;
    }

    testStep++;
}