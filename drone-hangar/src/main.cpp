#include <Arduino.h>
#include "config.hpp"
#include "kernel/Scheduler.hpp"
#include "kernel/Logger.hpp"
#include "kernel/Task.hpp"
#include "kernel/MsgService.hpp"
#include "model/HWPlatform.hpp"
#include "model/Context.hpp"

#define BASE_PERIOD_MS 50

// Uncomment the following line to enable hardware testing mode
// #define __TESTING_HW__

Scheduler sched;
HWPlatform* pHWPlatform;
Context* pContext;

void setup() {
  MsgService.iinit();
  sched.init(BASE_PERIOD_MS);

  Logger.log(":::::: Drone Hangar ::::::");
  
  pHWPlatform = new HWPlatform();
  pHWPlatform->init();

#ifndef __TESTING_HW__
  pContext = new Context();
  
  // Tasks
  // TODO: add tasks here
#endif

#ifdef __TESTING_HW__
  /* Testing */
  Task* pTestHWTask = new TestHWTask(pHWPlatform);
  pTestHWTask->init(2000);
  sched.addTask(pTestHWTask);
#endif
}

void loop() {
  sched.schedule();
}
