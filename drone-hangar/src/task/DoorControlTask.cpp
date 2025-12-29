#include "task/DoorControlTask.hpp"
#include <Arduino.hpp>
#include "config.hpp"
#include "kernel/Logger.hpp"

#define DOOR_OPEN_ANGLE 180
#define DOOR_CLOSED_ANGLE 0

DoorControlTask::DoorControlTask(Context* ctx, ServoMotor* motor) {
    pContext = ctx;
    pDoorMotor = motor;
    setSate(DOOR_CLOSED);
    pDoorMotor->setAngle(DOOR_CLOSED_ANGLE);
}

void DoorControlTask::tick() {
    switch(state) {
        case CLOSED:
            if (checkAndSetJustEntered()) {
                pContext->setDoorClosed();
                this->log("CLOSED");
            }

            if (pContext->isOpenDoorRequested()) {
                this->setSate(OPENING);
            }
            break;
        case OPENING:
            if (checkAndSetJustEntered()) {
                this->log("OPENING");
            }    

            this->openDoorStep();

            if (pContext->isCloseDoorRequested()) {
                this->setSate(CLOSING);
            } else if (isDoorOpened()) {
                this->setSate(OPEN);
            }
            break;
        case OPEN:
            if (checkAndSetJustEntered()) {
                this->log("OPEN");
                pContext->setDoorOpened();
            }

            if (pContext->isCloseDoorRequested()) {
                this->setSate(CLOSING);
            }
            break;
        case CLOSING:
            if (checkAndSetJustEntered()) {
                this->log("CLOSING");
            }

            this->closeDoorStep();

            if (isDoorClosed()) {
                this->setSate(CLOSED);
            }
            break;
    }
}

bool DoorControlTask::isDoorOpened() {
    return this->pDoorMotor >= DOOR_OPEN_ANGLE;
}

bool DoorControlTask::isDoorClosed() {
    //TODO
}

void DoorControlTask::openDoorStep() {
    //TODO
}

void DoorControlTask::closeDoorStep() {
    //TODO
}

void DoorControlTask::setSate(int s) {
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
