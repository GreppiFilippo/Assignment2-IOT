#include <Arduino.h>

#include "config.hpp"
#include "kernel/Logger.hpp"
#include "kernel/MsgService.hpp"
#include "kernel/Scheduler.hpp"
#include "kernel/Task.hpp"
#include "model/Context.hpp"
#include "model/HWPlatform.hpp"
#include "task/BlinkingTask.hpp"
#include "task/DoorControlTask.hpp"
#include "task/DroneTask.hpp"
#include "task/HangarTask.hpp"
#include "task/LCDTask.hpp"
#include "task/MSGTask.hpp"

#define BASE_PERIOD_MS 50

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
        new HangarTask(pHWPlatform->getTempSensor(), pHWPlatform->getButton(), pContext);
    pTempAlarmTask->init(TEMP_ALARM_TASK_PERIOD);

    Task* pDroneTask = new DroneTask(pContext, pHWPlatform->getL1(), pHWPlatform->getL3(),
                                     pHWPlatform->getPresenceSensor());
    pDroneTask->init(DRONE_TASK_PERIOD);

    Task* pBlinkingTask = new BlinkingTask(pHWPlatform->getL2(), pContext);
    pBlinkingTask->init(L2_BLINK_PERIOD);

    Task* pDoorControlTask = new DoorControlTask(pContext, pHWPlatform->getMotor());
    pDoorControlTask->init(DOOR_CONTROL_TASK_PERIOD);

    Task* pHangarTask =
        new HangarTask(pHWPlatform->getTempSensor(), pHWPlatform->getButton(), pContext);

    pHangarTask->init(HANGAR_TASK_PERIOD);

    Task* pLcdTask = new LCDTask(pHWPlatform->getLCD(), pContext);
    pLcdTask->init(LCD_TASK_PERIOD);

    Task* pMSGTask = new MSGTask(pContext, &MsgService);
    pMSGTask->init(MSG_TASK_PERIOD);

    sched.addTask(pTempAlarmTask);
    sched.addTask(pDroneTask);
    sched.addTask(pBlinkingTask);
    sched.addTask(pDoorControlTask);
    sched.addTask(pHangarTask);
    sched.addTask(pLcdTask);
    sched.addTask(pMSGTask);

#ifdef __TESTING_HW__
    /* Testing */
    Task* pTestHWTask = new TestHWTask(pHWPlatform);
    pTestHWTask->init(2000);
    sched.addTask(pTestHWTask);
#endif
}

void loop() { sched.schedule(); }
