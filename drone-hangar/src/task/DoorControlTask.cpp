#include "task/DoorControlTask.hpp"

#include "config.hpp"
#include "kernel/Logger.hpp"

#define DOOR_OPEN_ANGLE 180
#define DOOR_CLOSED_ANGLE 0

DoorControlTask::DoorControlTask(Context* ctx, ServoMotor* motor)
{
    pContext = ctx;
    pDoorMotor = motor;
    setState(CLOSED);
    pDoorMotor->setPosition(DOOR_CLOSED_ANGLE);
}

void DoorControlTask::tick()
{
    switch (state)
    {
        case CLOSED:
            if (checkAndSetJustEntered())
            {
                pContext->setDoorClosed();
                this->log("CLOSED");
            }

            if (pContext->isOpenDoorRequested())
            {
                this->setState(OPENING);
            }
            break;
        case OPENING:
            if (checkAndSetJustEntered())
            {
                this->log("OPENING");
            }

            this->openDoorStep();

            if (pContext->isCloseDoorRequested())
            {
                this->setState(CLOSING);
            }
            else if (isDoorOpened())
            {
                this->setState(OPEN);
            }
            break;
        case OPEN:
            if (checkAndSetJustEntered())
            {
                this->log("OPEN");
                pContext->setDoorOpened();
            }

            if (pContext->isCloseDoorRequested())
            {
                this->setState(CLOSING);
            }
            break;
        case CLOSING:
            if (checkAndSetJustEntered())
            {
                this->log("CLOSING");
            }

            this->closeDoorStep();

            if (isDoorClosed())
            {
                this->setState(CLOSED);
            }
            break;
    }
}

bool DoorControlTask::isDoorOpened()
{
    // TODO
}

bool DoorControlTask::isDoorClosed()
{
    // TODO
}

void DoorControlTask::openDoorStep()
{
    // TODO
}

void DoorControlTask::closeDoorStep()
{
    // TODO
}

void DoorControlTask::setState(State state)
{
    state = state;
    stateTimestamp = millis();
    justEntered = true;
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
