#include "LightSensorImpl.hpp"
#include "Arduino.h"

LightSensorImpl::LightSensorImpl(int pin){
  this->pin = pin;
}

double LightSensorImpl::getLightIntensity(){
  int value = analogRead(pin);
  double valueInVolt = ((double) value) * 5/1024;
  return valueInVolt/5.0;  
}
