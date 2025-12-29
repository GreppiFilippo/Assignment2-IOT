# 🚁 Smart Drone Hangar - Task Refactoring Guide

## 📌 Panoramica

Questo documento descrive come ristrutturare il sistema Drone Hangar per implementare un'architettura task-based coerente, scalabile e conforme ai requisiti.

---

## 🎯 Obiettivi del Refactoring

1. **Completare le task mancanti** per implementare tutte le funzionalità richieste
2. **Correggere le inconsistenze** tra task esistenti e Context
3. **Implementare la FSM principale** del sistema
4. **Separare responsabilità** per migliorare testabilità e manutenibilità
5. **Preparare il sistema per espansioni future**

---

## 🗂️ Struttura Task Finale

```
Task (8 totali):
├── TemperatureMonitoringTask   [NUOVA]
├── DroneDetectionTask           [NUOVA]
├── DoorControlTask              [OK - già implementata]
├── HangarStateMachineTask       [NUOVA - sostituisce HangarTask]
├── BlinkingTask                 [DA MODIFICARE]
├── LedControlTask               [NUOVA]
├── DisplayTask                  [NUOVA]
└── SerialCommunicationTask      [NUOVA]
```

---

## 📝 Modifiche al Context

### File:  `drone-hangar/src/model/Context.hpp`

#### 1. Aggiungere metodi per BlinkingTask

```cpp
// Aggiungi nella sezione SYSTEM STATE (dopo riga 76)

bool shouldBlinkL2() { 
    return currentSystemState == TAKE_OFF || currentSystemState == LANDING; 
}
```

#### 2. Aggiungere stati temperatura

```cpp
// Aggiungi dopo riga 11
enum TempState { TEMP_NORMAL, PRE_ALARM, ALARM };

// Aggiungi nelle variabili private (dopo riga 24)
TempState currentTempState;
unsigned long tempThresholdStartTime;

// Aggiungi nel costruttore (dopo riga 46)
currentTempState = TEMP_NORMAL;
tempThresholdStartTime = 0;

// Aggiungi nuovi metodi nella sezione ALARMS (dopo riga 83)
void setTempState(TempState state) { currentTempState = state; }
TempState getTempState() { return currentTempState; }
bool isTempNormal() { return currentTempState == TEMP_NORMAL; }

void setTempThresholdStartTime(unsigned long time) { tempThresholdStartTime = time; }
unsigned long getTempThresholdStartTime() { return tempThresholdStartTime; }
```

#### 3. Aggiungere tracking tempo sensori

```cpp
// Aggiungi nelle variabili private (dopo riga 30)
unsigned long distanceStableStartTime;
bool distanceStableTracking;

// Aggiungi nel costruttore (dopo riga 47)
distanceStableStartTime = 0;
distanceStableTracking = false;

// Aggiungi nuovi metodi nella sezione SENSOR DATA (dopo riga 112)
void startDistanceTracking() { 
    distanceStableTracking = true; 
    distanceStableStartTime = millis();
}
void stopDistanceTracking() { 
    distanceStableTracking = false; 
}
bool isDistanceTracking() { return distanceStableTracking; }
unsigned long getDistanceStableTime() { 
    return distanceStableTracking ? (millis() - distanceStableStartTime) : 0; 
}
```

#### 4. Aggiungere flag per reset button

```cpp
// Aggiungi nelle variabili private (dopo riga 33)
bool resetPressed;

// Aggiungi nel costruttore (dopo riga 47)
resetPressed = false;

// Aggiungi nuovi metodi dopo UTILITY (dopo riga 121)
void signalReset() { resetPressed = true; }
bool isResetPressed() {
    if (resetPressed) {
        resetPressed = false;
        return true;
    }
    return false;
}
```

---

## 🆕 Task da Creare

### 1. TemperatureMonitoringTask

**File:** `drone-hangar/src/task/TemperatureMonitoringTask.hpp`

```cpp
#ifndef __TEMPERATURE_MONITORING_TASK__
#define __TEMPERATURE_MONITORING_TASK__

#include "kernel/Task.hpp"
#include "model/Context.hpp"
#include "devices/TempSensor.hpp"
#include <Arduino.h>

class TemperatureMonitoringTask : public Task {
private: 
    Context* pContext;
    TempSensor* pTempSensor;
    
    const float TEMP1 = 30.0;  // Pre-alarm threshold
    const float TEMP2 = 35.0;  // Alarm threshold
    const unsigned long T3 = 5000;  // 5s for pre-alarm
    const unsigned long T4 = 3000;  // 3s for alarm
    
    unsigned long preAlarmStartTime;
    unsigned long alarmStartTime;
    bool trackingPreAlarm;
    bool trackingAlarm;

public:
    TemperatureMonitoringTask(Context* ctx, TempSensor* sensor);
    void init(unsigned long period);
    void tick();
};

#endif
```

**File:** `drone-hangar/src/task/TemperatureMonitoringTask.cpp`

```cpp
#include "task/TemperatureMonitoringTask.hpp"
#include "kernel/Logger.hpp"

TemperatureMonitoringTask::TemperatureMonitoringTask(Context* ctx, TempSensor* sensor) 
    : pContext(ctx), pTempSensor(sensor) {
    trackingPreAlarm = false;
    trackingAlarm = false;
}

void TemperatureMonitoringTask::init(unsigned long period) {
    Task::init(period);
}

void TemperatureMonitoringTask::tick() {
    float temp = pTempSensor->getTemperature();
    pContext->updateTemperature(temp);
    
    Context::SystemState sysState = pContext->getSystemState();
    bool droneInside = (sysState == Context::DRONE_INSIDE || 
                        sysState == Context::TAKE_OFF || 
                        sysState == Context::LANDING);
    
    if (! droneInside) {
        // Reset tracking if drone is out
        trackingPreAlarm = false;
        trackingAlarm = false;
        if (pContext->getTempState() != Context::TEMP_NORMAL) {
            pContext->setTempState(Context::TEMP_NORMAL);
        }
        return;
    }
    
    // Check for ALARM (Temp2)
    if (temp >= TEMP2) {
        if (! trackingAlarm) {
            trackingAlarm = true;
            alarmStartTime = millis();
        } else if (millis() - alarmStartTime >= T4) {
            if (pContext->getTempState() != Context::ALARM) {
                Logger. log("ALARM:  Temperature > TEMP2 for T4 seconds");
                pContext->setTempState(Context:: ALARM);
                pContext->setAlarm(true);
                pContext->requestCloseDoor();
            }
        }
        return;  // Skip pre-alarm check
    } else {
        trackingAlarm = false;
    }
    
    // Check for PRE-ALARM (Temp1)
    if (temp >= TEMP1) {
        if (!trackingPreAlarm) {
            trackingPreAlarm = true;
            preAlarmStartTime = millis();
        } else if (millis() - preAlarmStartTime >= T3) {
            if (pContext->getTempState() == Context::TEMP_NORMAL) {
                Logger.log("PRE-ALARM: Temperature > TEMP1 for T3 seconds");
                pContext->setTempState(Context::PRE_ALARM);
                pContext->setPreAlarm(true);
            }
        }
    } else {
        trackingPreAlarm = false;
        if (pContext->getTempState() == Context::PRE_ALARM) {
            Logger.log("Temperature back to normal");
            pContext->setTempState(Context::TEMP_NORMAL);
            pContext->setPreAlarm(false);
        }
    }
}
```

---

### 2. DroneDetectionTask

**File:** `drone-hangar/src/task/DroneDetectionTask.hpp`

```cpp
#ifndef __DRONE_DETECTION_TASK__
#define __DRONE_DETECTION_TASK__

#include "kernel/Task.hpp"
#include "model/Context.hpp"
#include "devices/Pir.hpp"
#include "devices/Sonar.hpp"
#include <Arduino.h>

class DroneDetectionTask :  public Task {
private:
    Context* pContext;
    Pir* pPir;
    Sonar* pSonar;
    
    const int SONAR_SAMPLES = 3;
    float distanceBuffer[3];
    int bufferIndex;
    
    float getFilteredDistance();

public:
    DroneDetectionTask(Context* ctx, Pir* pir, Sonar* sonar);
    void init(unsigned long period);
    void tick();
};

#endif
```

**File:** `drone-hangar/src/task/DroneDetectionTask.cpp`

```cpp
#include "task/DroneDetectionTask.hpp"

DroneDetectionTask::DroneDetectionTask(Context* ctx, Pir* pir, Sonar* sonar)
    : pContext(ctx), pPir(pir), pSonar(sonar) {
    bufferIndex = 0;
    for (int i = 0; i < SONAR_SAMPLES; i++) {
        distanceBuffer[i] = 0;
    }
}

void DroneDetectionTask::init(unsigned long period) {
    Task::init(period);
}

float DroneDetectionTask::getFilteredDistance() {
    // Median filter
    float sorted[SONAR_SAMPLES];
    for (int i = 0; i < SONAR_SAMPLES; i++) {
        sorted[i] = distanceBuffer[i];
    }
    // Simple bubble sort
    for (int i = 0; i < SONAR_SAMPLES - 1; i++) {
        for (int j = 0; j < SONAR_SAMPLES - i - 1; j++) {
            if (sorted[j] > sorted[j + 1]) {
                float temp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = temp;
            }
        }
    }
    return sorted[SONAR_SAMPLES / 2];  // Return median
}

void DroneDetectionTask::tick() {
    // Update PIR
    bool pirDetected = pPir->isDetected();
    pContext->setPirActive(pirDetected);
    
    // Update Sonar with filtering
    distanceBuffer[bufferIndex] = pSonar->getDistance();
    bufferIndex = (bufferIndex + 1) % SONAR_SAMPLES;
    
    float filteredDistance = getFilteredDistance();
    pContext->updateDistance(filteredDistance);
}
```

---

### 3. HangarStateMachineTask

**File:** `drone-hangar/src/task/HangarStateMachineTask.hpp`

```cpp
#ifndef __HANGAR_STATE_MACHINE_TASK__
#define __HANGAR_STATE_MACHINE_TASK__

#include "kernel/Task.hpp"
#include "model/Context.hpp"
#include <Arduino.h>

class HangarStateMachineTask : public Task {
private:
    Context* pContext;
    
    const float D1 = 100.0;  // Distance threshold for "drone out" (cm)
    const float D2 = 10.0;   // Distance threshold for "drone landed" (cm)
    const unsigned long T1 = 3000;  // 3s stability for takeoff
    const unsigned long T2 = 3000;  // 3s stability for landing
    
    void setState(Context::SystemState newState);
    void log(const String& msg);

public:
    HangarStateMachineTask(Context* ctx);
    void init(unsigned long period);
    void tick();
};

#endif
```

**File:** `drone-hangar/src/task/HangarStateMachineTask.cpp`

```cpp
#include "task/HangarStateMachineTask.hpp"
#include "kernel/Logger.hpp"

HangarStateMachineTask::HangarStateMachineTask(Context* ctx) : pContext(ctx) {}

void HangarStateMachineTask::init(unsigned long period) {
    Task::init(period);
    pContext->setSystemState(Context:: DRONE_INSIDE);
}

void HangarStateMachineTask::setState(Context::SystemState newState) {
    pContext->setSystemState(newState);
}

void HangarStateMachineTask::log(const String& msg) {
    Logger.log("[HSM] " + msg);
}

void HangarStateMachineTask:: tick() {
    Context::SystemState currentState = pContext->getSystemState();
    
    // Handle RESET in ALARM state
    if (currentState == Context::ALARM) {
        if (pContext->isResetPressed()) {
            log("RESET pressed - returning to normal");
            pContext->setAlarm(false);
            pContext->setTempState(Context::TEMP_NORMAL);
            // Assume drone is inside after reset
            setState(Context::DRONE_INSIDE);
        }
        return;  // Block all other operations
    }
    
    // Handle ALARM condition
    if (pContext->getTempState() == Context::ALARM) {
        log("ALARM activated!");
        setState(Context::ALARM);
        return;
    }
    
    bool preAlarmActive = pContext->getTempState() == Context::PRE_ALARM;
    float distance = pContext->getDistance();
    
    switch (currentState) {
    case Context::DRONE_INSIDE:  {
        // Check for takeoff request
        if (pContext->isTakeoffRequested() && !preAlarmActive) {
            log("Takeoff requested - opening door");
            pContext->requestOpenDoor();
            setState(Context::TAKE_OFF);
            pContext->startDistanceTracking();
        }
        break;
    }
    
    case Context::TAKE_OFF:  {
        // Wait for door to open fully
        if (pContext->isDoorOpen()) {
            // Check if drone has left (distance > D1 for T1 seconds)
            if (distance > D1) {
                if (pContext->getDistanceStableTime() >= T1) {
                    log("Drone has left - closing door");
                    pContext->requestCloseDoor();
                    pContext->stopDistanceTracking();
                    setState(Context:: DRONE_OUT);
                }
            } else {
                // Reset tracking if distance goes back below threshold
                pContext->startDistanceTracking();
            }
        }
        break;
    }
    
    case Context::DRONE_OUT: {
        // Check for landing request
        if (pContext->isLandingRequested() && !preAlarmActive) {
            if (pContext->isPirActive()) {
                log("Landing requested with PIR detection - opening door");
                pContext->requestOpenDoor();
                setState(Context::LANDING);
                pContext->startDistanceTracking();
            } else {
                log("Landing requested but PIR not detecting drone!");
            }
        }
        break;
    }
    
    case Context::LANDING: {
        // Wait for door to open fully
        if (pContext->isDoorOpen()) {
            // Check if drone has landed (distance < D2 for T2 seconds)
            if (distance < D2) {
                if (pContext->getDistanceStableTime() >= T2) {
                    log("Drone has landed - closing door");
                    pContext->requestCloseDoor();
                    pContext->stopDistanceTracking();
                    setState(Context::DRONE_INSIDE);
                }
            } else {
                // Reset tracking if distance goes back above threshold
                pContext->startDistanceTracking();
            }
        }
        break;
    }
    
    case Context:: ALARM: 
        // Already handled above
        break;
    }
}
```

---

### 4. LedControlTask

**File:** `drone-hangar/src/task/LedControlTask.hpp`

```cpp
#ifndef __LED_CONTROL_TASK__
#define __LED_CONTROL_TASK__

#include "kernel/Task.hpp"
#include "model/Context.hpp"
#include "devices/Led.hpp"
#include <Arduino.h>

class LedControlTask : public Task {
private:
    Context* pContext;
    Led* pL1;  // Green - Drone inside
    Led* pL3;  // Red - Alarm

public:
    LedControlTask(Context* ctx, Led* l1, Led* l3);
    void init(unsigned long period);
    void tick();
};

#endif
```

**File:** `drone-hangar/src/task/LedControlTask.cpp`

```cpp
#include "task/LedControlTask.hpp"

LedControlTask::LedControlTask(Context* ctx, Led* l1, Led* l3)
    : pContext(ctx), pL1(l1), pL3(l3) {}

void LedControlTask::init(unsigned long period) {
    Task::init(period);
    pL1->switchOn();  // Start with drone inside
    pL3->switchOff();
}

void LedControlTask:: tick() {
    Context::SystemState state = pContext->getSystemState();
    
    // L1: ON when drone is inside
    if (state == Context::DRONE_INSIDE) {
        pL1->switchOn();
    } else {
        pL1->switchOff();
    }
    
    // L3: ON during alarm
    if (state == Context::ALARM) {
        pL3->switchOn();
    } else {
        pL3->switchOff();
    }
}
```

---

### 5. DisplayTask

**File:** `drone-hangar/src/task/DisplayTask.hpp`

```cpp
#ifndef __DISPLAY_TASK__
#define __DISPLAY_TASK__

#include "kernel/Task.hpp"
#include "model/Context.hpp"
#include "devices/Lcd.hpp"
#include <Arduino.h>

class DisplayTask : public Task {
private: 
    Context* pContext;
    Lcd* pLcd;
    String lastMessage;
    
    void displayMessage(const String& line1, const String& line2 = "");

public:
    DisplayTask(Context* ctx, Lcd* lcd);
    void init(unsigned long period);
    void tick();
};

#endif
```

**File:** `drone-hangar/src/task/DisplayTask.cpp`

```cpp
#include "task/DisplayTask.hpp"

DisplayTask::DisplayTask(Context* ctx, Lcd* lcd)
    : pContext(ctx), pLcd(lcd) {
    lastMessage = "";
}

void DisplayTask::init(unsigned long period) {
    Task::init(period);
    pLcd->clear();
}

void DisplayTask::displayMessage(const String& line1, const String& line2) {
    String fullMessage = line1 + "|" + line2;
    if (fullMessage != lastMessage) {
        pLcd->clear();
        pLcd->setCursor(0, 0);
        pLcd->print(line1);
        if (line2.length() > 0) {
            pLcd->setCursor(0, 1);
            pLcd->print(line2);
        }
        lastMessage = fullMessage;
    }
}

void DisplayTask::tick() {
    Context::SystemState state = pContext->getSystemState();
    
    switch (state) {
    case Context::DRONE_INSIDE: 
        displayMessage("DRONE INSIDE");
        break;
        
    case Context::TAKE_OFF:
        displayMessage("TAKE OFF");
        break;
        
    case Context::DRONE_OUT:
        displayMessage("DRONE OUT");
        break;
        
    case Context:: LANDING:
        displayMessage("LANDING");
        break;
        
    case Context::ALARM:  {
        float temp = pContext->getTemperature();
        displayMessage("ALARM", "Temp:  " + String(temp, 1) + "C");
        break;
    }
    }
}
```

---

### 6. SerialCommunicationTask

**File:** `drone-hangar/src/task/SerialCommunicationTask.hpp`

```cpp
#ifndef __SERIAL_COMMUNICATION_TASK__
#define __SERIAL_COMMUNICATION_TASK__

#include "kernel/Task.hpp"
#include "model/Context.hpp"
#include "kernel/MsgService.hpp"
#include <Arduino.h>

class SerialCommunicationTask : public Task {
private:
    Context* pContext;
    MsgService* pMsgService;
    
    void parseCommand(const String& msg);
    void sendStatus();
    
    unsigned long lastStatusSent;
    const unsigned long STATUS_INTERVAL = 500;  // Send status every 500ms

public: 
    SerialCommunicationTask(Context* ctx, MsgService* msgService);
    void init(unsigned long period);
    void tick();
};

#endif
```

**File:** `drone-hangar/src/task/SerialCommunicationTask. cpp`

```cpp
#include "task/SerialCommunicationTask.hpp"

SerialCommunicationTask:: SerialCommunicationTask(Context* ctx, MsgService* msgService)
    : pContext(ctx), pMsgService(msgService) {
    lastStatusSent = 0;
}

void SerialCommunicationTask::init(unsigned long period) {
    Task::init(period);
}

void SerialCommunicationTask::parseCommand(const String& msg) {
    if (msg == "TAKEOFF") {
        pContext->requestTakeoff();
    } else if (msg == "LANDING") {
        pContext->requestLanding();
    }
}

void SerialCommunicationTask::sendStatus() {
    // Format: STATE|DISTANCE|TEMP|DOOR|ALARM
    String stateStr;
    switch (pContext->getSystemState()) {
        case Context::DRONE_INSIDE:  stateStr = "INSIDE"; break;
        case Context:: TAKE_OFF: stateStr = "TAKEOFF"; break;
        case Context::DRONE_OUT: stateStr = "OUT"; break;
        case Context:: LANDING: stateStr = "LANDING"; break;
        case Context::ALARM: stateStr = "ALARM"; break;
    }
    
    String doorStr = pContext->isDoorOpen() ? "OPEN" : "CLOSED";
    String alarmStr = pContext->isAlarmActive() ? "1" : "0";
    
    String statusMsg = stateStr + "|" + 
                       String(pContext->getDistance(), 1) + "|" +
                       String(pContext->getTemperature(), 1) + "|" +
                       doorStr + "|" + alarmStr;
    
    pMsgService->sendMsg(statusMsg);
}

void SerialCommunicationTask::tick() {
    // Check for incoming commands
    if (pMsgService->isMsgAvailable()) {
        Msg* msg = pMsgService->receiveMsg();
        String content = msg->getContent();
        parseCommand(content);
        delete msg;
    }
    
    // Send periodic status updates
    if (millis() - lastStatusSent >= STATUS_INTERVAL) {
        sendStatus();
        lastStatusSent = millis();
    }
    
    // Send immediate ALARM notification
    if (pContext->getSystemState() == Context::ALARM && 
        millis() - lastStatusSent >= 100) {
        sendStatus();
        lastStatusSent = millis();
    }
}
```

---

## ✏️ Modifiche alle Task Esistenti

### 1. BlinkingTask - Correggere logica

**File:** `drone-hangar/src/task/BlinkingTask.cpp`

Sostituire il metodo `tick()` con:

```cpp
void BlinkingTask::tick(){
    switch (state){   
    case IDLE:  {
        if (this->checkAndSetJustEntered()){
            pLed->switchOff();
            Logger.log(F("[BT] IDLE"));
        }
        if (pContext->shouldBlinkL2()){  // CORREZIONE QUI
            setState(OFF);
        }
        break;
    }
    case OFF:  {
        if (this->checkAndSetJustEntered()){
            pLed->switchOff();
            Logger.log(F("[BT] OFF"));
        }
        if (! pContext->shouldBlinkL2()){  // CORREZIONE QUI
            setState(IDLE);
        } else {
             setState(ON);
        }
        break;
    }
    case ON:  {
        if (this->checkAndSetJustEntered()){
            pLed->switchOn();
            Logger.log(F("[BT] ON"));
        }
        if (!pContext->shouldBlinkL2()){  // CORREZIONE QUI
            setState(IDLE);
        } else {
            setState(OFF);
        }
        break;
    }
    }
}
```

### 2. Rimuovere HangarTask. cpp

Elimina completamente `drone-hangar/src/task/HangarTask.cpp` e `HangarTask.hpp` - saranno sostituiti da `HangarStateMachineTask`.

### 3. Rimuovere TestHWTask (opzionale)

Se non serve per testing, elimina `TestHWTask.cpp` e `TestHWTask.hpp`.

---

## 🔧 Aggiornare main.cpp

**File:** `drone-hangar/src/main.cpp`

```cpp
#include <Arduino.h>
#include "kernel/Scheduler.hpp"
#include "kernel/MsgService.hpp"
#include "model/Context.hpp"

// Tasks
#include "task/TemperatureMonitoringTask.hpp"
#include "task/DroneDetectionTask.hpp"
#include "task/DoorControlTask.hpp"
#include "task/HangarStateMachineTask. hpp"
#include "task/BlinkingTask.hpp"
#include "task/LedControlTask.hpp"
#include "task/DisplayTask.hpp"
#include "task/SerialCommunicationTask.hpp"

// Devices
#include "devices/Led.hpp"
#include "devices/Button.hpp"
#include "devices/ServoMotor.hpp"
#include "devices/Sonar.hpp"
#include "devices/Pir.hpp"
#include "devices/TempSensor.hpp"
#include "devices/Lcd.hpp"

#include "config. hpp"

Scheduler scheduler;
Context context;
MsgService msgService;

void setup() {
    Serial.begin(9600);
    msgService.init();
    
    // Initialize devices
    Led* l1 = new Led(LED_L1_PIN);
    Led* l2 = new Led(LED_L2_PIN);
    Led* l3 = new Led(LED_L3_PIN);
    Button* resetBtn = new Button(BUTTON_RESET_PIN);
    ServoMotor* doorMotor = new ServoMotor(SERVO_DOOR_PIN);
    Sonar* sonar = new Sonar(SONAR_ECHO_PIN, SONAR_TRIG_PIN);
    Pir* pir = new Pir(PIR_PIN);
    TempSensor* tempSensor = new TempSensor(TEMP_SENSOR_PIN);
    Lcd* lcd = new Lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
    
    // Initialize tasks
    Task* tempTask = new TemperatureMonitoringTask(&context, tempSensor);
    tempTask->init(200);  // Check every 200ms
    
    Task* detectionTask = new DroneDetectionTask(&context, pir, sonar);
    detectionTask->init(100);  // Check every 100ms
    
    Task* doorTask = new DoorControlTask(&context, doorMotor);
    doorTask->init(20);  // 20ms for smooth servo control
    
    Task* fsmTask = new HangarStateMachineTask(&context);
    fsmTask->init(100);  // Main FSM runs every 100ms
    
    Task* blinkTask = new BlinkingTask(l2, &context);
    blinkTask->init(250);  // 250ms = 0.5s period (ON + OFF)
    
    Task* ledTask = new LedControlTask(&context, l1, l3);
    ledTask->init(100);
    
    Task* displayTask = new DisplayTask(&context, lcd);
    displayTask->init(200);
    
    Task* serialTask = new SerialCommunicationTask(&context, &msgService);
    serialTask->init(50);  // Check serial frequently
    
    // Add tasks to scheduler
    scheduler.init(10);  // Base period:  10ms
    scheduler.addTask(tempTask);
    scheduler.addTask(detectionTask);
    scheduler.addTask(doorTask);
    scheduler.addTask(fsmTask);
    scheduler.addTask(blinkTask);
    scheduler.addTask(ledTask);
    scheduler.addTask(displayTask);
    scheduler.addTask(serialTask);
}

void loop() {
    scheduler.schedule();
    
    // Handle RESET button (can be polled here or use interrupt)
    // TODO: Add button handling
}
```

---

## 📊 Diagramma Interazione Task

```
┌──────────────────────────────────────┐
│           SCHEDULER (10ms)           │
└──────────┬───────────────────────────┘
           │
    ┌──────┴──────┐
    │   CONTEXT   │
    └──────┬──────┘
           │
    ┌──────┴──────────────────────────────────────┐
    │                                              │
┌───▼────────┐  ┌────────────┐  ┌───────────────┐│
│Temperature │  │Drone       │  │Door           ││
│Monitoring  │  │Detection   │  │Control        ││
│(200ms)     │  │(100ms)     │  │(20ms)         ││
└────┬───────┘  └─────┬──────┘  └──────┬────────┘│
     │                │                 │         │
     │writes temp     │writes PIR/dist  │writes   │
     │writes states   │                 │doorState│
     │                │                 │         │
     └────────────────┴─────────────────┴─────────┤
                      │                           │
              ┌───────▼──────────┐                │
              │Hangar State      │◄───reads all   │
              │Machine (100ms)   │    sensors     │
              └───────┬──────────┘                │
                      │writes systemState         │
                      │issues commands            │
         ┌────────────┼────────────┬──────────────┤
         │            │            │              │
    ┌────▼─────┐ ┌───▼────┐ ┌────▼──────┐ ┌─────▼──────┐
    │Blinking  │ │Led     │ │Display    │ │Serial      │
    │L2(250ms) │ │Control │ │(200ms)    │ │Comm(50ms)  │
    └──────────┘ │(100ms) │ └───────────┘ └────────────┘
                 └────────┘
```

---

## ✅ Checklist di Implementazione

### Fase 1: Context
- [ ] Aggiungere `shouldBlinkL2()`
- [ ] Aggiungere `TempState` enum e metodi
- [ ] Aggiungere tracking tempo distanza
- [ ] Aggiungere gestione reset button

### Fase 2: Task Prioritarie
- [ ] Creare `TemperatureMonitoringTask`
- [ ] Creare `DroneDetectionTask`
- [ ] Creare `HangarStateMachineTask`
- [ ] Modificare `BlinkingTask`

### Fase 3: Task UI/Comunicazione
- [ ] Creare `LedControlTask`
- [ ] Creare `DisplayTask`
- [ ] Creare `SerialCommunicationTask`

### Fase 4: Integrazione
- [ ] Rimuovere `HangarTask` vecchia
- [ ] Aggiornare `main.cpp`
- [ ] Verificare pin configuration in `config.hpp`
- [ ] Testare ogni task individualmente
- [ ] Testare sistema integrato

### Fase 5: Testing Scenari
- [ ] Test:  Takeoff completo
- [ ] Test: Landing completo
- [ ] Test:  Pre-allarme temperatura
- [ ] Test: Allarme temperatura
- [ ] Test: Reset da allarme
- [ ] Test:  Blinking LED durante operazioni
- [ ] Test: Comunicazione seriale con DRU

---

## 🚀 Espansioni Future Supportate

Grazie a questa architettura, è facile aggiungere: 

1. **BatteryMonitoringTask** - Monitoraggio batteria drone
2. **WeatherMonitoringTask** - Sensori meteo esterni
3. **MultiDroneTask** - Supporto per più droni (array di Context)
4. **DataLoggerTask** - Salvataggio su SD card
5. **WiFiCommunicationTask** - Controllo remoto via WiFi
6. **CameraTask** - Verifica visiva atterraggio

Ogni nuova funzionalità = nuova Task + eventuali aggiunte al Context! 

---

## 📝 Note Implementative

### Priorità Periodi Task
```
DoorControlTask:         20ms  (controllo servo smooth)
SerialCommunicationTask: 50ms  (responsività comandi)
DroneDetectionTask:     100ms (sensori)
HangarStateMachineTask: 100ms (logica principale)
LedControlTask:         100ms (indicatori)
DisplayTask:            200ms (LCD update)
TemperatureMonitoringTask:  200ms (temperatura lenta)
BlinkingTask:           250ms (periodo 0.5s = 250ms ON + 250ms OFF)
```

### Gestione Button Reset
Puoi implementarlo in due modi: 

**Opzione A - Polling nel loop():**
```cpp
void loop() {
    scheduler.schedule();
    
    static Button resetBtn(BUTTON_RESET_PIN);
    if (resetBtn.isPressed()) {
        context.signalReset();
    }
}
```

**Opzione B - Interrupt (più elegante):**
```cpp
volatile bool resetFlag = false;

void resetISR() {
    resetFlag = true;
}

void setup() {
    // ...
    pinMode(BUTTON_RESET_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_RESET_PIN), resetISR, FALLING);
}

void loop() {
    scheduler.schedule();
    
    if (resetFlag) {
        context.signalReset();
        resetFlag = false;
    }
}
```

---

## 🎓 Principi Architetturali Applicati

1. **Single Responsibility**:  Ogni task ha UN solo scopo
2. **Separation of Concerns**: Sensori, logica, UI separati
3. **Data-Driven**: Context è unica fonte di verità
4. **Non-Blocking**: Nessun delay, solo state machine
5. **Scalability**: Aggiungere task non impatta le esistenti
6. **Testability**:  Ogni task testabile in isolamento

---

**Buon lavoro!  🚁**