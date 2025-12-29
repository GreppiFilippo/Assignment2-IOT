#ifndef __BLINKING_TASK__
#define __BLINKING_TASK__

#include "kernel/Task.hpp"
#include "model/Context.hpp"
#include "devices/Led.hpp"
#include <Arduino.hpp>

class BlinkingTask: public Task {

public:
  BlinkingTask(Led* pLed, Context* pContext); 
  void tick();

private:  
  void setState(int state);
  long elapsedTimeInState();
  void log(const String& msg);
  
  bool checkAndSetJustEntered();
  
  enum { IDLE, OFF, ON } state;
  long stateTimestamp;
  bool justEntered;

  Led* pLed;
  Context* pContext;
};

#endif /* __BLINKING_TASK__ */