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

#define BASE_PERIOD_MS 50
#define DRONE_TASK_PERIOD 50
#define DOOR_CONTROL_TASK_PERIOD 50
#define HANGAR_TASK_PERIOD 200
#define DISTANCE_TASK_PERIOD 50
#define LCD_TASK_PERIOD 50
#define MSG_TASK_PERIOD 50

#ifdef _MEMEORY_DEBUG_
/* ======== MACRO DI DEBUG ======== */
#define DEBUG_MEM(label)        \
    Serial.print(F("[MEM] "));  \
    Serial.print(F(label));     \
    Serial.print(F(": "));      \
    Serial.print(freeMemory()); \
    Serial.println(F(" bytes free"))

#define CHECK_PTR(ptr, name)                                 \
    if (ptr == nullptr)                                      \
    {                                                        \
        Serial.print(F("!!! CRASH: Memoria esaurita per ")); \
        Serial.println(F(name));                             \
        while (1);                                           \
    }

#endif

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
#ifdef _MEMEORY_DEBUG_
    Serial.begin(115200);
    while (!Serial);
    Serial.println(F("\n--- SYSTEM BOOT ---"));
    DEBUG_MEM("Inizio Setup");
#endif

    /* ======== Inizializzazioni Kernel ======== */
    MsgService.init(BAUD_RATE);
    sched.init(BASE_PERIOD_MS);
#ifdef _MEMEORY_DEBUG_
    DEBUG_MEM("Kernel OK");
#endif
    /* ======== Hardware Platform ======== */
    pHWPlatform = new HWPlatform();
#ifdef _MEMEORY_DEBUG_
    CHECK_PTR(pHWPlatform, "HWPlatform");
#endif
    pHWPlatform->init();
#ifdef _MEMEORY_DEBUG_
    DEBUG_MEM("HWPlatform OK");
#endif
#ifndef __TESTING_HW__
    /* ======== Context (Il cuore dei dati) ======== */
    pContext = new Context();
#ifdef _MEMEORY_DEBUG_
    CHECK_PTR(pContext, "Context");
    DEBUG_MEM("Context OK");
#endif
    /* ======== Inizializzazione Task ======== */

    Task* pDroneTask =
        new DroneTask(pContext, pHWPlatform->getL1(), pHWPlatform->getPresenceSensor());
#ifdef _MEMEORY_DEBUG_
    CHECK_PTR(pDroneTask, "DroneTask");
#endif
    pDroneTask->init(DRONE_TASK_PERIOD);

    Task* pLcdTask = new LCDTask(pHWPlatform->getLCD(), pContext);
#ifdef _MEMEORY_DEBUG_
    CHECK_PTR(pLcdTask, "LCDTask");
#endif
    pLcdTask->init(LCD_TASK_PERIOD);

    Task* pMSGTask = new MsgTask(pContext, &MsgService);
#ifdef _MEMEORY_DEBUG_
    CHECK_PTR(pMSGTask, "MsgTask");
#endif
    pMSGTask->init(MSG_TASK_PERIOD);

    Task* pHangarTask = new HangarTask(pHWPlatform->getTempSensor(), pHWPlatform->getButton(),
                                       pHWPlatform->getL3(), pContext);
#ifdef _MEMEORY_DEBUG_
    CHECK_PTR(pHangarTask, "HangarTask");
#endif
    pHangarTask->init(HANGAR_TASK_PERIOD);

    Task* pBlinkingTask = new BlinkingTask(pHWPlatform->getL2(), pContext);
#ifdef _MEMEORY_DEBUG_
    CHECK_PTR(pBlinkingTask, "BlinkingTask");
#endif
    pBlinkingTask->init(L2_BLINK_PERIOD);

    Task* pDoorControlTask = new DoorControlTask(pContext, pHWPlatform->getMotor());
#ifdef _MEMEORY_DEBUG_
    CHECK_PTR(pDoorControlTask, "DoorTask");
#endif
    pDoorControlTask->init(DOOR_CONTROL_TASK_PERIOD);

    Task* pDistanceTask = new DistanceTask(pHWPlatform->getProximitySensor(), pContext);
#ifdef _MEMEORY_DEBUG_
    CHECK_PTR(pDistanceTask, "DistanceTask");
#endif
    pDistanceTask->init(DISTANCE_TASK_PERIOD);

    /* ======== Registrazione Task nello Scheduler ======== */
    sched.addTask(pHangarTask);
    sched.addTask(pBlinkingTask);
    sched.addTask(pDoorControlTask);
    sched.addTask(pDistanceTask);
    sched.addTask(pDroneTask);
    sched.addTask(pLcdTask);
    sched.addTask(pMSGTask);

#ifdef _MEMEORY_DEBUG_
    DEBUG_MEM("Setup Completato");
#endif

    Logger.log(F(":::::: Drone Hangar Ready ::::::"));
#endif

#ifdef __TESTING_HW__
    Task* pTestHWTask = new TestHWTask(pHWPlatform);
    pTestHWTask->init(2000);
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