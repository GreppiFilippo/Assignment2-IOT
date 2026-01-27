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
                L1->switchOn();
                pContext->stopBlink();
                Logger.log(F("[Drone] REST"));
            }

            pContext->setLCDMessage(pContext->isAlarmActive() ? LCD_ALARM_STATE : LCD_REST_STATE);

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
                pContext->setLCDMessage(LCD_TAKING_OFF_STATE);
                pContext->blink();
                Logger.log(F("[Drone] TAKING OFF"));
            }

            if (pContext->isAlarmActive() && pContext->isDroneIn())
            {
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
                L1->switchOff();
                pContext->stopBlink();
                Logger.log(F("[Drone] OPERATING"));
            }

            pContext->setLCDMessage(pContext->isAlarmActive() ? LCD_ALARM_STATE
                                                              : LCD_OPERATING_STATE);

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
                pContext->setLCDMessage(LCD_LANDING_STATE);
                pContext->blink();
                Logger.log(F("[Drone] LANDING"));
            }

            if (pContext->isAlarmActive() && !pContext->isDroneIn())
            {
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