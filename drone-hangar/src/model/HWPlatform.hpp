#ifndef __HW_PLATFORM__
#define __HW_PLATFORM__

#include "config.hpp"
#include "devices/Button.hpp"
#include "devices/Led.hpp"
#include "devices/ServoMotor.hpp"

class HWPlatform {

public:
  HWPlatform();
  void init();
  void test();

  Button* getButton();
  Led*  getLed();
  ServoMotor* getMotor();

private:
  Button* pButton;
  Led* pLed;
  ServoMotor* pMotor;
  
};

#endif
