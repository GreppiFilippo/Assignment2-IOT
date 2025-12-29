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
    Context();
    
    // === DOOR CONTROL ===
    void requestOpenDoor();
    bool isOpenDoorRequested();
    void requestCloseDoor();
    bool isCloseDoorRequested(); 
    void setDoorState(DoorState state);
    DoorState getDoorState();
    bool isDoorOpen();
    bool isDoorClosed();
    
    // === SYSTEM STATE ===
    void setSystemState(SystemState state);
    SystemState getSystemState();
    
    // === ALARMS ===
    void setAlarm(bool active);
    bool isAlarmActive();
    void setPreAlarm(bool active);
    bool isPreAlarmActive();
    
    // === COMMANDS ===
    void requestTakeoff();
    bool isTakeoffRequested();
    void requestLanding();
    bool isLandingRequested();
    
    // === SENSOR DATA ===
    void updateDistance(float dist);
    float getDistance();
    void updateTemperature(float temp);
    float getTemperature();
    void setPirActive(bool active);
    bool isPirActive();
    
    // === UTILITY ===
    bool canExecuteTakeoff();
    bool canExecuteLanding();
};

#endif
