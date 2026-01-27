#ifndef __DOOR_CONTROL_TASK__
#define __DOOR_CONTROL_TASK__

#include <Arduino.h>

#include "devices/ServoMotor.hpp"
#include "kernel/Task.hpp"
#include "model/Context.hpp"

/**
 * Task responsible for controlling the hangar door using a servo motor.
 *
 * State machine:
 * - CLOSED: Door is fully closed, waiting for open request
 * - OPENING: Door is gradually opening over DOOR_OPERATION_TIME ms
 * - OPEN: Door is fully open, waiting for close request or alarm
 * - CLOSING: Door is gradually closing over DOOR_OPERATION_TIME ms
 *
 * The door opens when Context::isOpenDoorRequested() is true
 * The door closes when Context::isCloseDoorRequested() is true or on alarm
 * Updates Context door state (OPEN/CLOSED) when fully opened/closed
 */
class DoorControlTask : public Task
{
   private:
    Context* pContext;
    ServoMotor* pDoorMotor;

    /**
     * @brief Door states for the FSM
     *
     */
    enum State
    {
        CLOSED,
        OPENING,
        OPEN,
        CLOSING
    } state;

    long stateTimestamp;
    bool justEntered;
    int currentPos;

    void setState(State state);
    long elapsedTimeInState();
    void log(const char* msg);
    bool checkAndSetJustEntered();

    bool isDoorOpen();
    bool isDoorClosed();

   public:
    /**
     * @brief Construct a new Door Control Task object
     *
     * @param ctx the context shared across tasks
     * @param motor the servo motor controlling the door
     */
    DoorControlTask(Context* ctx, ServoMotor* motor);

    /**
     * @brief Task execution method called by the scheduler when the task runs.
     *
     */
    void tick() override;
};

#endif