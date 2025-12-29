#include "task/DoorControlTask.hpp"
#include <Arduino.hpp>
#include "config.hpp"
#include "kernel/Logger.hpp"

#define DOOR_OPEN_ANGLE 90
#define DOOR_CLOSED_ANGLE 0
#define DOOR_OPERATION_TIME 2000  // 2 seconds to open/close

DoorControlTask::DoorControlTask(Context* ctx, ServoMotor* motor) : pContext(ctx), pDoorMotor(motor) {
    setState(DOOR_CLOSED);
    currentAngle = DOOR_CLOSED_ANGLE;
}

void DoorControlTask::init(unsigned long period) {
    Task::init(period);
    pDoorMotor->on();
    pDoorMotor->setPosition(DOOR_CLOSED_ANGLE);
}

void DoorControlTask::tick() {
    switch (state) {
    case DOOR_CLOSED: {
        if (checkAndSetJustEntered()) {
            log("DOOR CLOSED");
            pDoorMotor->setPosition(DOOR_CLOSED_ANGLE);
            currentAngle = DOOR_CLOSED_ANGLE;
            pContext->setDoorState(Context::CLOSED);
        }
        
        // Check for open request
        if (pContext->isOpenDoorRequested()) {
            setState(DOOR_OPENING);
        }
        break;
    }
    
    case DOOR_OPENING: {
        if (checkAndSetJustEntered()) {
            log("DOOR OPENING");
        }
        
        // Calculate progress (0.0 to 1.0)
        long elapsed = elapsedTimeInState();
        float progress = (float)elapsed / DOOR_OPERATION_TIME;
        
        if (progress >= 1.0) {
            // Opening complete
            currentAngle = DOOR_OPEN_ANGLE;
            pDoorMotor->setPosition(DOOR_OPEN_ANGLE);
            setState(DOOR_OPEN);
        } else {
            // Gradual opening
            currentAngle = DOOR_CLOSED_ANGLE + (progress * (DOOR_OPEN_ANGLE - DOOR_CLOSED_ANGLE));
            pDoorMotor->setPosition(currentAngle);
        }
        
        // Emergency close for alarm
        if (pContext->isCloseDoorRequested()) {
            setState(DOOR_CLOSING);
        }
        break;
    }
    
    case DOOR_OPEN: {
        if (checkAndSetJustEntered()) {
            log("DOOR OPEN");
            pDoorMotor->setPosition(DOOR_OPEN_ANGLE);
            currentAngle = DOOR_OPEN_ANGLE;
            pContext->setDoorState(Context::OPEN);
        }
        
        // Check for close request or alarm
        if (pContext->isCloseDoorRequested()) {
            setState(DOOR_CLOSING);
        }
        break;
    }
    
    case DOOR_CLOSING: {
        if (checkAndSetJustEntered()) {
            log("DOOR CLOSING");
        }
        
        // Calculate progress (0.0 to 1.0)
        long elapsed = elapsedTimeInState();
        float progress = (float)elapsed / DOOR_OPERATION_TIME;
        
        if (progress >= 1.0) {
            // Closing complete
            currentAngle = DOOR_CLOSED_ANGLE;
            pDoorMotor->setPosition(DOOR_CLOSED_ANGLE);
            setState(DOOR_CLOSED);
        } else {
            // Gradual closing
            currentAngle = DOOR_OPEN_ANGLE - (progress * (DOOR_OPEN_ANGLE - DOOR_CLOSED_ANGLE));
            pDoorMotor->setPosition(currentAngle);
        }
        break;
    }
    }
}
        
void DoorControlTask::setState(int s) {
    state = s;
    stateTimestamp = millis();
    justEntered = true;
}

long DoorControlTask::elapsedTimeInState() {
    return millis() - stateTimestamp;
}

bool DoorControlTask::checkAndSetJustEntered() {
    bool bak = justEntered;
    if (justEntered) {
        justEntered = false;
    }
    return bak;
}

void DoorControlTask::log(const String& msg) {
    Serial.print("[DOOR] ");
    Serial.println(msg);
}
