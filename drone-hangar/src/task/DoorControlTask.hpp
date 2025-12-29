#ifndef __DOOR_CONTROL_TASK__
#define __DOOR_CONTROL_TASK__

#include "kernel/Task.hpp"
#include "model/Context.hpp"
#include "devices/ServoMotor.hpp"
#include <Arduino.h>

/**
 * Task responsible for controlling the hangar door using a servo motor.
 * 
 * State machine:
 * - DOOR_CLOSED: Door is fully closed, waiting for open request
 * - DOOR_OPENING: Door is gradually opening over DOOR_OPERATION_TIME ms
 * - DOOR_OPEN: Door is fully open, waiting for close request or alarm
 * - DOOR_CLOSING: Door is gradually closing over DOOR_OPERATION_TIME ms
 * 
 * The door opens when Context::isOpenDoorRequested() is true
 * The door closes when Context::isCloseDoorRequested() is true or on alarm
 * Updates Context door state (OPEN/CLOSED) when fully opened/closed
 */
class DoorControlTask : public Task {
    private:
        Context* pContext;
        ServoMotor* pDoorMotor;

        void setState(int state);
        long elapsedTimeInState();
        void log(const String& msg);
        bool checkAndSetJustEntered();
        
        enum { DOOR_CLOSED, DOOR_OPENING, DOOR_OPEN, DOOR_CLOSING } state;
        long stateTimestamp;
        bool justEntered;

        int currentAngle;

    public:
        DoorControlTask(Context* ctx, ServoMotor* motor);

        void init(unsigned long period);

        void tick();
};

#endif