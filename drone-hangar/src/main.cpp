#include <Arduino.h>

#include "config.hpp"
#include "kernel/Logger.hpp"
#include "kernel/MsgService.hpp"
#include "kernel/Scheduler.hpp"
#include "kernel/Task.hpp"
#include "model/Context.hpp"
#include "model/HWPlatform.hpp"
#include "task/TempAlarmTask.hpp"

#define BASE_PERIOD_MS 50
#define TEMP_ALARM_TASK_PERIOD

// Uncomment the following line to enable hardware testing mode
// #define __TESTING_HW__
#ifdef __TESTING_HW__
#include "task/TestHWTask.h"
#endif

Scheduler sched;
HWPlatform* pHWPlatform;
Context* pContext;

void setup()
{
    MsgService.init();
    sched.init(BASE_PERIOD_MS);

    Logger.log(":::::: Drone Hangar ::::::");

    pHWPlatform = new HWPlatform();
    pHWPlatform->init();

#ifndef __TESTING_HW__
    pContext = new Context();

    // Tasks
    // TODO: add tasks here

    Task* pTempAlarmTask =
        new TempAlarmTask(pHWPlatform->getTempSensor(), pHWPlatform->getButton(), pContext);
    pTempAlarmTask->init(TEMP_ALARM_TASK_PERIOD);

    sched.addTask(pTempAlarmTask);

#endif

#ifdef __TESTING_HW__
    /* Testing */
    Task* pTestHWTask = new TestHWTask(pHWPlatform);
    pTestHWTask->init(2000);
    sched.addTask(pTestHWTask);
#endif
}

void loop() { sched.schedule(); }
