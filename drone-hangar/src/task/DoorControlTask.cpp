#include "task/DoorControlTask.hpp"

#include "config.hpp"
#include "kernel/Logger.hpp"

#define DOOR_OPEN_ANGLE 180
#define DOOR_CLOSED_ANGLE 0
//TODO: da misurare!!!!!!!!!
#define MOVING_TIME 300 //tempo in cui si apre/chiude. Per isClosed/isOpen. 

DoorControlTask::DoorControlTask(Context* ctx, ServoMotor* motor)
{
    this->pContext = ctx;
    this->pDoorMotor = motor;
    setState(CLOSED);
    this->pDoorMotor->setPosition(DOOR_CLOSED_ANGLE);
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

            if (pContext->openDoorReq())
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

            if (pContext->closeDoorReq())
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

            if (pContext->closeDoorReq())
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
    return elapsedTimeInState() > MOVING_TIME;
}

bool DoorControlTask::isDoorClosed()
{
    return elapsedTimeInState() > MOVING_TIME;
}

void DoorControlTask::openDoorStep()
{
    this->pDoorMotor->setPosition(DOOR_OPEN_ANGLE);
}

void DoorControlTask::closeDoorStep()
{
    this->pDoorMotor->setPosition(DOOR_CLOSED_ANGLE);
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
