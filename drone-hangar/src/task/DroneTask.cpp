#include "task/DroneTask.hpp"

#include "config.hpp"
#include "devices/Led.hpp"
#include "kernel/Logger.hpp"

DroneTask::DroneTask(Context* pContext, Light* L1, PresenceSensor* presenceSensor)
{
    this->pContext = pContext;
    this->setState(REST);
    this->L1 = L1;
    this->presenceSensor = presenceSensor;
}

void DroneTask::sendState(const String& state)
{
    this->pContext->setJsonField("drone_state", state);
}

void DroneTask::tick()
{
    switch (this->state)
    {
        case REST:
            sendState(DRONE_REST_STATE);

            if (checkAndSetJustEntered())
            {
                pContext->closeDoor();
                L1->switchOn();
                pContext->stopBlink();
            }

            pContext->setLCDMessage(pContext->isAlarmActive() ? LCD_ALARM_STATE : LCD_REST_STATE);

            if (pContext->consumeCommand(CommandType::OPEN) &&
                !(pContext->isPreAlarmActive() || pContext->isAlarmActive()))
            {
                setState(TAKING_OFF);
            }
            break;

        case TAKING_OFF:
            Logger.log(F(DRONE_TAKING_OFF_STATE));
            sendState(DRONE_TAKING_OFF_STATE);

            if (checkAndSetJustEntered())
            {
                pContext->openDoor();
                pContext->setLCDMessage(LCD_TAKING_OFF_STATE);
                pContext->blink();
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
            Logger.log(F(DRONE_OPERATING_STATE));
            sendState(DRONE_OPERATING_STATE);

            if (checkAndSetJustEntered())
            {
                pContext->closeDoor();
                L1->switchOff();
                pContext->stopBlink();
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
            Logger.log(F(DRONE_LANDING_STATE));
            sendState(DRONE_LANDING_STATE);

            if (checkAndSetJustEntered())
            {
                pContext->openDoor();
                pContext->setLCDMessage(LCD_LANDING_STATE);
                pContext->blink();
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