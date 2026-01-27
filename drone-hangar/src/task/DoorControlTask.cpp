#include "task/DoorControlTask.hpp"

#include "config.hpp"
#include "kernel/Logger.hpp"

DoorControlTask::DoorControlTask(Context* ctx, ServoMotor* motor)
{
    this->pContext = ctx;
    this->pDoorMotor = motor;
    this->currentPos = 0;
    this->pDoorMotor->setPosition(this->currentPos);
    this->setState(CLOSED);
}

void DoorControlTask::tick()
{
    switch (this->state)
    {
        case CLOSED:
        {
            if (this->checkAndSetJustEntered())
            {
                this->pContext->setDoorClosed();
                Logger.log(F("[DOOR] CLOSED"));
            }

            if (this->pContext->openDoorReq())
            {
                this->setState(OPENING);
            }
            break;
        }

        case OPENING:
        {
            if (this->checkAndSetJustEntered())
            {
                Logger.log(F("[DOOR] OPENING"));
            }

            long dt = this->elapsedTimeInState();
            if (dt > MOVING_TIME)
                dt = MOVING_TIME;

            this->currentPos = ((float)dt / MOVING_TIME) * DOOR_OPEN_ANGLE;
            this->pDoorMotor->setPosition(this->currentPos);

            if (this->pContext->closeDoorReq())
            {
                this->setState(CLOSING);
            }
            else if (this->isDoorOpen())
            {
                this->setState(OPEN);
            }
            break;
        }

        case OPEN:
        {
            if (this->checkAndSetJustEntered())
            {
                this->pContext->setDoorOpened();
                Logger.log(F("[DOOR] OPEN"));
            }

            if (this->pContext->closeDoorReq())
            {
                this->setState(CLOSING);
            }
            break;
        }

        case CLOSING:
        {
            if (this->checkAndSetJustEntered())
            {
                Logger.log(F("[DOOR] CLOSING"));
            }

            long dt = this->elapsedTimeInState();
            if (dt > MOVING_TIME)
                dt = MOVING_TIME;

            this->currentPos = DOOR_OPEN_ANGLE - ((float)dt / MOVING_TIME) * DOOR_OPEN_ANGLE;
            this->pDoorMotor->setPosition(this->currentPos);

            if (this->isDoorClosed())
            {
                this->setState(CLOSED);
            }
            break;
        }
    }
}

bool DoorControlTask::isDoorOpen() { return this->currentPos == DOOR_OPEN_ANGLE; }
bool DoorControlTask::isDoorClosed() { return this->currentPos == 0; }

void DoorControlTask::setState(State state)
{
    this->state = state;
    this->stateTimestamp = millis();
    this->justEntered = true;
}

long DoorControlTask::elapsedTimeInState() { return millis() - this->stateTimestamp; }

bool DoorControlTask::checkAndSetJustEntered()
{
    bool bak = this->justEntered;
    if (this->justEntered)
    {
        this->justEntered = false;
    }
    return bak;
}
