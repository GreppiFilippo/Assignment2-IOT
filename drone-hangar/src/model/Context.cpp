#include "model/Context.hpp"

Context::Context() {
    currentSystemState = DRONE_INSIDE;
    currentDoorState = CLOSED;
    openDoorRequested = false;
    closeDoorRequested = false;
    alarmActive = false;
    preAlarmActive = false;
    takeoffRequested = false;
    landingRequested = false;
    currentDistance = 0.0;
    currentTemperature = 25.0;
    pirActive = false;
}

// === DOOR CONTROL ===
void Context::requestOpenDoor() { openDoorRequested = true; }
bool Context::isOpenDoorRequested() {
    if (openDoorRequested) {
        openDoorRequested = false;
        return true;
    }
    return false;
}

void Context::requestCloseDoor() { closeDoorRequested = true; }
bool Context::isCloseDoorRequested() {
    if (closeDoorRequested) {
        closeDoorRequested = false;
        return true;
    }
    return false;
}

void Context::setDoorState(DoorState state) { currentDoorState = state; }
DoorState Context::getDoorState() { return currentDoorState; }
bool Context::isDoorOpen() { return currentDoorState == OPEN; }
bool Context::isDoorClosed() { return currentDoorState == CLOSED; }

// === SYSTEM STATE ===
void Context::setSystemState(SystemState state) { currentSystemState = state; }
SystemState Context::getSystemState() { return currentSystemState; }

// === ALARMS ===
void Context::setAlarm(bool active) { alarmActive = active; }
bool Context::isAlarmActive() { return alarmActive; }

void Context::setPreAlarm(bool active) { preAlarmActive = active; }
bool Context::isPreAlarmActive() { return preAlarmActive; }

// === COMMANDS ===
void Context::requestTakeoff() { takeoffRequested = true; }
bool Context::isTakeoffRequested() {
    if (takeoffRequested) {
        takeoffRequested = false;
        return true;
    }
    return false;
}

void Context::requestLanding() { landingRequested = true; }
bool Context::isLandingRequested() {
    if (landingRequested) {
        landingRequested = false;
        return true;
    }
    return false;
}

// === SENSOR DATA ===
void Context::updateDistance(float dist) { currentDistance = dist; }
float Context::getDistance() { return currentDistance; }

void Context::updateTemperature(float temp) { currentTemperature = temp; }
float Context::getTemperature() { return currentTemperature; }

void Context::setPirActive(bool active) { pirActive = active; }
bool Context::isPirActive() { return pirActive; }

// === UTILITY ===
bool Context::canExecuteTakeoff() {
    return !alarmActive && currentSystemState == DRONE_INSIDE;
}

bool Context::canExecuteLanding() {
    return !alarmActive && currentSystemState == DRONE_OUT && pirActive;
}
