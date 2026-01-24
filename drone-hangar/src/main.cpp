#include <Arduino.h>

#include "config.hpp"
#include "kernel/Logger.hpp"
#include "kernel/MsgService.hpp"
#include "kernel/Scheduler.hpp"
#include "kernel/Task.hpp"
#include "model/Context.hpp"
#include "model/HWPlatform.hpp"
#include "task/BlinkingTask.hpp"
#include "task/DistanceTask.hpp"
#include "task/DoorControlTask.hpp"
#include "task/DroneTask.hpp"
#include "task/HangarTask.hpp"
#include "task/LCDTask.hpp"
#include "task/MSGTask.hpp"

#define BASE_PERIOD_MS 50
#define DRONE_TASK_PERIOD 50
#define DOOR_CONTROL_TASK_PERIOD 50
#define HANGAR_TASK_PERIOD 50
#define DISTANCE_TASK_PERIOD 50
#define LCD_TASK_PERIOD 50
#define MSG_TASK_PERIOD 50

// Uncomment the following line to enable hardware testing mode
// #define __TESTING_HW__
#ifdef __TESTING_HW__
#include "task/TestHWTask.hpp"
#endif

Scheduler sched;
HWPlatform* pHWPlatform;
Context* pContext;

void setup()
{
    /* ======== Serial Initialization ======== */
    MsgService.init();

    /* ======== Scheduler Initialization ======== */
    sched.init(BASE_PERIOD_MS);

    /* ======== Hardware Platform Initialization ======== */
    pHWPlatform = new HWPlatform();
    pHWPlatform->init();

#ifndef __TESTING_HW__
    /* ======== Context Initialization ======== */
    pContext = new Context();

    /* ======== Tasks initialization ======== */
    Task* pDroneTask =
        new DroneTask(pContext, pHWPlatform->getL1(), pHWPlatform->getPresenceSensor());
    pDroneTask->init(DRONE_TASK_PERIOD);

    Task* pLcdTask = new LCDTask(pHWPlatform->getLCD(), pContext);
    pLcdTask->init(LCD_TASK_PERIOD);

    Task* pMSGTask = new MsgTask(pContext, &MsgService);
    pMSGTask->init(MSG_TASK_PERIOD);

    Task* pHangarTask = new HangarTask(pHWPlatform->getTempSensor(), pHWPlatform->getButton(),
                                       pHWPlatform->getL3(), pContext);
    pHangarTask->init(HANGAR_TASK_PERIOD);

    Task* pBlinkingTask = new BlinkingTask(pHWPlatform->getL2(), pContext);
    pBlinkingTask->init(L2_BLINK_PERIOD);

    Task* pDoorControlTask = new DoorControlTask(pContext, pHWPlatform->getMotor());
    pDoorControlTask->init(DOOR_CONTROL_TASK_PERIOD);

    Task* pDistanceTask = new DistanceTask(pHWPlatform->getProximitySensor(), pContext);
    pDistanceTask->init(DISTANCE_TASK_PERIOD);

    /* ======== Adding Tasks to Scheduler ======== */
    sched.addTask(pHangarTask);
    sched.addTask(pBlinkingTask);
    sched.addTask(pDoorControlTask);
    sched.addTask(pDistanceTask);
    sched.addTask(pDroneTask);
    sched.addTask(pLcdTask);
    sched.addTask(pMSGTask);

    Logger.log(":::::: Drone Hangar ::::::");
#endif

#ifdef __TESTING_HW__
    /* Testing */
    Task* pTestHWTask = new TestHWTask(pHWPlatform);
    pTestHWTask->init(2000);
    sched.addTask(pTestHWTask);

    Logger.log(":::::: Hardware Testing Mode ::::::");
#endif
}

void loop() { sched.schedule(); }
