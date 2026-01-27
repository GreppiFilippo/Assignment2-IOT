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

/* ======== COSTANTI DEI PERIODI (Le ho rimesse tutte!) ======== */
#define BASE_PERIOD_MS 50
#define DRONE_TASK_PERIOD 50
#define DOOR_CONTROL_TASK_PERIOD 50
#define HANGAR_TASK_PERIOD 200
#define DISTANCE_TASK_PERIOD 50
#define LCD_TASK_PERIOD 50
#define MSG_TASK_PERIOD 50
// Nota: L2_BLINK_PERIOD di solito è in config.hpp, 
// se ti dà errore aggiungi qui: #define L2_BLINK_PERIOD 500

/* ======== MACRO DI DEBUG ======== */
#define DEBUG_MEM(label) \
    Serial.print(F("[MEM] ")); Serial.print(F(label)); \
    Serial.print(F(": ")); Serial.print(freeMemory()); Serial.println(F(" bytes free"))

#define CHECK_PTR(ptr, name) \
    if (ptr == nullptr) { \
        Serial.print(F("!!! CRASH: Memoria esaurita per ")); Serial.println(F(name)); \
        while(1); \
    }

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

void setup() {
    Serial.begin(115200);
    while (!Serial); 
    Serial.println(F("\n--- SYSTEM BOOT ---"));
    DEBUG_MEM("Inizio Setup");

    /* ======== Inizializzazioni Kernel ======== */
    MsgService.init();
    sched.init(BASE_PERIOD_MS);
    DEBUG_MEM("Kernel OK");

    /* ======== Hardware Platform ======== */
    pHWPlatform = new HWPlatform();
    CHECK_PTR(pHWPlatform, "HWPlatform");
    pHWPlatform->init();
    DEBUG_MEM("HWPlatform OK");

#ifndef __TESTING_HW__
    /* ======== Context (Il cuore dei dati) ======== */
    pContext = new Context();
    CHECK_PTR(pContext, "Context");
    DEBUG_MEM("Context OK");

    /* ======== Inizializzazione Task ======== */
    
    Task* pDroneTask = new DroneTask(pContext, pHWPlatform->getL1(), pHWPlatform->getPresenceSensor());
    CHECK_PTR(pDroneTask, "DroneTask");
    pDroneTask->init(DRONE_TASK_PERIOD);
    
    Task* pLcdTask = new LCDTask(pHWPlatform->getLCD(), pContext);
    CHECK_PTR(pLcdTask, "LCDTask");
    pLcdTask->init(LCD_TASK_PERIOD);

    Task* pMSGTask = new MsgTask(pContext, &MsgService);
    CHECK_PTR(pMSGTask, "MsgTask");
    pMSGTask->init(MSG_TASK_PERIOD);

    Task* pHangarTask = new HangarTask(pHWPlatform->getTempSensor(), pHWPlatform->getButton(),
                                       pHWPlatform->getL3(), pContext);
    CHECK_PTR(pHangarTask, "HangarTask");
    pHangarTask->init(HANGAR_TASK_PERIOD);

    Task* pBlinkingTask = new BlinkingTask(pHWPlatform->getL2(), pContext);
    CHECK_PTR(pBlinkingTask, "BlinkingTask");
    pBlinkingTask->init(L2_BLINK_PERIOD);

    Task* pDoorControlTask = new DoorControlTask(pContext, pHWPlatform->getMotor());
    CHECK_PTR(pDoorControlTask, "DoorTask");
    pDoorControlTask->init(DOOR_CONTROL_TASK_PERIOD);

    Task* pDistanceTask = new DistanceTask(pHWPlatform->getProximitySensor(), pContext);
    CHECK_PTR(pDistanceTask, "DistanceTask");
    pDistanceTask->init(DISTANCE_TASK_PERIOD);

    /* ======== Registrazione Task nello Scheduler ======== */
    sched.addTask(pHangarTask);
    sched.addTask(pBlinkingTask);
    sched.addTask(pDoorControlTask);
    sched.addTask(pDistanceTask);
    sched.addTask(pDroneTask);
    sched.addTask(pLcdTask);
    sched.addTask(pMSGTask);

    DEBUG_MEM("Setup Completato");
    Logger.log(F(":::::: Drone Hangar Ready ::::::"));
#endif

#ifdef __TESTING_HW__
    Task* pTestHWTask = new TestHWTask(pHWPlatform);
    pTestHWTask->init(2000);
    sched.addTask(pTestHWTask);
    Logger.log(F(":::::: Hardware Testing Mode ::::::"));
#endif
}

void loop() {
    sched.schedule();

    // Heartbeat della memoria ogni 5 secondi
    if (millis() - lastMemCheck > 5000) {
        lastMemCheck = millis();
        Serial.print(F("[DEBUG] RAM libera: "));
        Serial.print(freeMemory());
        Serial.println(F(" bytes"));
    }
}