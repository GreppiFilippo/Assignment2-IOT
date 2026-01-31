#include "task/DroneTask.hpp"

#include "config.hpp"
#include "devices/Led.hpp"
#include "kernel/Logger.hpp"

DroneTask::DroneTask(Context* pContext, Light* L1, PresenceSensor* presenceSensor)
{
    this->pContext = pContext;
    this->L1 = L1;
    this->presenceSensor = presenceSensor;
    this->setState(REST);
    L1->switchOn();
}

void DroneTask::tick()
{
    switch (this->state)
    {
        case REST:
            if (checkAndSetJustEntered())
            {
                pContext->setDroneState(REST);
                pContext->closeDoor();
                pContext->stopBlink();
                Logger.log(F("[Drone] REST"));
            }

            if (!pContext->isAlarmActive())
            {
                pContext->setLCDMessage(LCD_REST_STATE);
            }

            if (pContext->consumeCommand(CommandType::OPEN) &&
                !(pContext->isPreAlarmActive() || pContext->isAlarmActive()))
            {
                setState(TAKING_OFF);
            }
            break;

        case TAKING_OFF:
            if (checkAndSetJustEntered())
            {
                pContext->setDroneState(TAKING_OFF);
                pContext->openDoor();
                pContext->requestTakeoffCheck();
                pContext->setLCDMessage(LCD_TAKING_OFF_STATE);
                pContext->blink();
                Logger.log(F("[Drone] TAKING OFF"));
            }

            if (pContext->isAlarmActive() && pContext->isDroneIn())
            {
                pContext->closeTakeoffCheck();
                setState(REST);
            }
            else if (!pContext->isDroneIn())
            {
                setState(OPERATING);
            }
            break;

        case OPERATING:
            if (checkAndSetJustEntered())
            {
                pContext->setDroneState(OPERATING);
                pContext->closeDoor();
                pContext->stopBlink();
                Logger.log(F("[Drone] OPERATING"));
            }

            if (!pContext->isAlarmActive())
            {
                pContext->setLCDMessage(LCD_OPERATING_STATE);
            }

            if (pContext->consumeCommand(CommandType::OPEN) &&
                !(pContext->isPreAlarmActive() || pContext->isAlarmActive()) &&
                presenceSensor->isDetected())
            {
                setState(LANDING);
            }
            break;

        case LANDING:
            if (checkAndSetJustEntered())
            {
                pContext->setDroneState(LANDING);
                pContext->openDoor();
                pContext->requestLandingCheck();
                pContext->setLCDMessage(LCD_LANDING_STATE);
                pContext->blink();
                Logger.log(F("[Drone] LANDING"));
            }

            if (pContext->isAlarmActive() && !pContext->isDroneIn())
            {
                pContext->closeLandingCheck();
                setState(OPERATING);
            }
            else if (pContext->isDroneIn())
            {
                setState(REST);
            }
            break;
    }
}

void DroneTask::setState(State state)
{
    this->state = state;
    this->stateTimestamp = millis();
    this->justEntered = true;
}

long DroneTask::elapsedTimeInState() { return millis() - stateTimestamp; }

bool DroneTask::checkAndSetJustEntered()
{
    bool bak = justEntered;
    if (justEntered)
        justEntered = false;
    return bak;
}
