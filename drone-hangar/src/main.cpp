#include <Arduino.h>
#include <MemoryFree.h>

#include "config.hpp"
#include "kernel/Logger.hpp"
#include "kernel/MsgService.hpp"
#include "kernel/Scheduler.hpp"
#include "kernel/Task.hpp"
#include "model/Context.hpp"
#include "model/HWPlatform.hpp"

// Task Headers
#include "task/BlinkingTask.hpp"
#include "task/DistanceTask.hpp"
#include "task/DoorControlTask.hpp"
#include "task/DroneTask.hpp"
#include "task/HangarTask.hpp"
#include "task/LCDTask.hpp"
#include "task/MSGTask.hpp"

/* ======== VARIABILI GLOBALI ======== */
Scheduler sched;
HWPlatform* pHWPlatform;
Context* pContext;
unsigned long lastMemCheck = 0;

// Commenta/Decommenta per il testing hardware
// #define __TESTING_HW__
#ifdef __TESTING_HW__
#include "task/TestHWTask.hpp"
#endif


void setup()
{
    /* ======== Inizializzazioni Kernel ======== */
    MsgService.init(BAUD_RATE);
    sched.init(BASE_PERIOD_MS);

    /* ======== Hardware Platform ======== */
    pHWPlatform = new HWPlatform();
    pHWPlatform->init();
    #ifndef __TESTING_HW__
    /* ======== Context (Il cuore dei dati) ======== */
    pContext = new Context();
    /* ======== Inizializzazione Task ======== */

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

    /* ======== Registrazione Task nello Scheduler ======== */
    sched.addTask(pDroneTask);
    sched.addTask(pHangarTask);
    sched.addTask(pBlinkingTask);
    sched.addTask(pDoorControlTask);
    sched.addTask(pDistanceTask);
    sched.addTask(pLcdTask);
    sched.addTask(pMSGTask);
    Logger.log(F(":::::: Drone Hangar Ready ::::::"));
#endif

#ifdef __TESTING_HW__
    Task* pTestHWTask = new TestHWTask(pHWPlatform);
    pTestHWTask->init(200);  // Velocizzato a 200ms per un test fluido dei sensori
    sched.addTask(pTestHWTask);
    Logger.log(F(":::::: Hardware Testing Mode ::::::"));
#endif
}

void loop()
{
    sched.schedule();

#ifdef _MEMEORY_DEBUG_
    // Memory heartbeat every 5 seconds
    if (millis() - lastMemCheck > 5000)
    {
        lastMemCheck = millis();
        Serial.print(F("[DEBUG] RAM libera: "));
        Serial.print(freeMemory());
        Serial.println(F(" bytes"));
    }
#endif
}