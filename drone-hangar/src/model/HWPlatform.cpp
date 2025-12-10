#include "HWPlatform.hpp"
#include <Arduino.h>
#include "devices/ButtonImpl.hpp"
#include "kernel/MsgService.hpp"
#include "config.hpp"
#include "devices/Led.hpp"
#include "devices/LightSensorImpl.hpp"
#include "devices/ServoMotorImpl.hpp"
#include "kernel/Logger.hpp"

void wakeUp(){}

HWPlatform::HWPlatform(){
  pButton = new ButtonImpl(BT_PIN);
  pLed = new Led(LED_PIN);
  pMotor = new ServoMotorImpl(MOTOR_PIN);
}


void HWPlatform::init(){
}

Button* HWPlatform::getButton(){
  return this->pButton;
}


Led*  HWPlatform::getLed(){
  return this->pLed;
}

ServoMotor* HWPlatform::getMotor(){
  return this->pMotor;
}

void HWPlatform::test(){
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

