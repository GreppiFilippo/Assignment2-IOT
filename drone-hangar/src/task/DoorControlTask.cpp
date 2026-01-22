#include "task/DoorControlTask.hpp"

#include "config.hpp"


DoorControlTask::DoorControlTask(Context* ctx, ServoMotor* motor)
{
    pContext = ctx;
    pDoorMotor = motor;
    currentPos = 0;
    pDoorMotor->setPosition(currentPos);
    pContext->openDoor();
    setState(CLOSED);
}

void DoorControlTask::tick()
{
    switch (state)
    {
        case CLOSED: {
            if (checkAndSetJustEntered()) {
                pContext->setDoorClosed();
                log("CLOSED");
            }

            if (pContext->openDoorReq()) {
                setState(OPENING);
            }
            break;
        }

        case OPENING: {
            if (checkAndSetJustEntered()) {
                log("OPENING");
            }

            long dt = elapsedTimeInState();
            if (dt > MOVING_TIME) dt = MOVING_TIME;

            currentPos = ((float)dt / MOVING_TIME) * DOOR_OPEN_ANGLE;
            pDoorMotor->setPosition(currentPos);

            if (pContext->closeDoorReq()) {
                setState(CLOSING);
            }
            else if (isDoorOpened()) {
                setState(OPEN);
            }
            break;
        }

        case OPEN: {
            if (checkAndSetJustEntered()) {
                pContext->setDoorOpened();
                log("OPEN");
            }

            if (pContext->closeDoorReq()) {
                setState(CLOSING);
            }
            break;
        }

        case CLOSING: {
            if (checkAndSetJustEntered()) {
                log("CLOSING");
            }

            long dt = elapsedTimeInState();
            if (dt > MOVING_TIME) dt = MOVING_TIME;

            currentPos = DOOR_OPEN_ANGLE -
                         ((float)dt / MOVING_TIME) * DOOR_OPEN_ANGLE;
            pDoorMotor->setPosition(currentPos);

            if (isDoorClosed()) {
                setState(CLOSED);
            }
            break;
        }
    }
}


bool DoorControlTask::isDoorOpened()
{
    return currentPos==DOOR_OPEN_ANGLE;
}

bool DoorControlTask::isDoorClosed()
{
    return currentPos==0;
}


void DoorControlTask::setState(State state)
{
    this->state = state;
    this->stateTimestamp = millis();
    this->justEntered = true;
}

long DoorControlTask::elapsedTimeInState() { return millis() - stateTimestamp; }

bool DoorControlTask::checkAndSetJustEntered()
{
    bool bak = justEntered;
    if (justEntered)
    {
        justEntered = false;
    }
    return bak;
}

void DoorControlTask::log(const String& msg)
{
    Serial.print("[DOOR] ");
    Serial.println(msg);
}
