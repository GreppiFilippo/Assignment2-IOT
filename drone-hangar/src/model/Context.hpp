#ifndef __CONTEXT__
#define __CONTEXT__

// TODO:
class Context {
public:
    // Door states
    enum DoorState { CLOSED, OPENING, OPEN, CLOSING };
    
    // System states  
    enum SystemState { DRONE_INSIDE, TAKE_OFF, DRONE_OUT, LANDING, ALARM };

private:
    // System state
    SystemState currentSystemState;
    DoorState currentDoorState;
    
    // Door control requests (auto-clear)
    bool openDoorRequested;
    bool closeDoorRequested;
    
    // System flags
    bool alarmActive;
    bool preAlarmActive;
    
    // Sensor data
    float currentDistance;
    float currentTemperature;
    bool pirActive;
    
    // Command requests (auto-clear)
    bool takeoffRequested;
    bool landingRequested;

public:
    Context() {
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
    void requestOpenDoor() { openDoorRequested = true; }
    bool isOpenDoorRequested() {
        if (openDoorRequested) {
            openDoorRequested = false;
            return true;
        }
        return false;
    }
    
    void requestCloseDoor() { closeDoorRequested = true; }
    bool isCloseDoorRequested() {
        if (closeDoorRequested) {
            closeDoorRequested = false;
            return true;
        }
        return false;
    }
    
    void setDoorState(DoorState state) { currentDoorState = state; }
    DoorState getDoorState() { return currentDoorState; }
    bool isDoorOpen() { return currentDoorState == OPEN; }
    bool isDoorClosed() { return currentDoorState == CLOSED; }
    
    // === SYSTEM STATE ===
    void setSystemState(SystemState state) { currentSystemState = state; }
    SystemState getSystemState() { return currentSystemState; }
    
    // === ALARMS ===
    void setAlarm(bool active) { alarmActive = active; }
    bool isAlarmActive() { return alarmActive; }
    
    void setPreAlarm(bool active) { preAlarmActive = active; }
    bool isPreAlarmActive() { return preAlarmActive; }
    
    // === COMMANDS ===
    void requestTakeoff() { takeoffRequested = true; }
    bool isTakeoffRequested() {
        if (takeoffRequested) {
            takeoffRequested = false;
            return true;
        }
        return false;
    }
    
    void requestLanding() { landingRequested = true; }
    bool isLandingRequested() {
        if (landingRequested) {
            landingRequested = false;
            return true;
        }
        return false;
    }
    
    // === SENSOR DATA ===
    void updateDistance(float dist) { currentDistance = dist; }
    float getDistance() { return currentDistance; }
    
    void updateTemperature(float temp) { currentTemperature = temp; }
    float getTemperature() { return currentTemperature; }
    
    void setPirActive(bool active) { pirActive = active; }
    bool isPirActive() { return pirActive; }
    
    // === UTILITY ===
    bool canExecuteTakeoff() {
        return !alarmActive && currentSystemState == DRONE_INSIDE;
    }
    
    bool canExecuteLanding() {
        return !alarmActive && currentSystemState == DRONE_OUT && pirActive;
    }
};

#endif
