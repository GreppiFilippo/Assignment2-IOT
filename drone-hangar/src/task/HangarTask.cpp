#include "task/HangarTask.hpp"

#include "config.hpp"
#include "kernel/Logger.hpp"

#define NORMAL_STATE_MSG "HANGAR_STATE:NORMAL"
#define ALARM_STATE_MSG "HANGAR_STATE:ALARM"

HangarTask::HangarTask(TempSensor* tempSensor, Button* resetButton, Context* pContext)
    : pContext(pContext), tempSensor(tempSensor), resetButton(resetButton)
{
    setState(NORMAL);
}

void HangarTask::setHangarState(const String& state)
{
    this->pContext->setJsonField("hangar_state", state);
}

void HangarTask::tick()
{
    this->temperature = tempSensor->getTemperature();
    switch (state)
    {
        case NORMAL:
            this->setHangarState("normal");

            if (this->checkAndSetJustEntered())
            {
                Logger.log(F("[HT] NORMAL"));
                this->pContext->setPreAlarm(false);
                this->pContext->setAlarm(false);
            }

            if (this->temperature >= TEMP1)
            {
                this->setState(TRACKING_PRE_ALARM);
            }
            break;
        case TRACKING_PRE_ALARM:
            this->setHangarState("normal");

            if (this->checkAndSetJustEntered())
            {
                Logger.log(F("[HT] TRACKING_PRE_ALARM"));
                this->startTime = millis();
            }

            if (this->temperature < TEMP1)
            {
                this->setState(NORMAL);
            }
            else if (millis() - this->startTime >= T3)
            {
                this->setState(PREALARM);
            }
            break;
        case PREALARM:
            this->setHangarState("normal");

            if (this->checkAndSetJustEntered())
            {
                Logger.log(F("[HT] PREALARM"));
                this->pContext->setPreAlarm(true);
            }

            if (this->temperature < TEMP1)
            {
                this->setState(NORMAL);
            }
            else if (this->temperature >= TEMP2)
            {
                this->setState(TRACKING_ALARM);
            }
            break;
        case TRACKING_ALARM:
            this->setHangarState("normal");

            if (this->checkAndSetJustEntered())
            {
                Logger.log(F("[HT] TRACKING_ALARM"));
                this->startTime = millis();
            }

            if (this->temperature < TEMP2)
            {
                this->setState(PREALARM);
            }
            else if (millis() - this->startTime >= T4)
            {
                this->setState(ALARM);
            }
            break;
        case ALARM:
            this->setHangarState("alarm");
            if (this->checkAndSetJustEntered())
            {
                Logger.log(F("[HT] ALARM"));
                this->pContext->setPreAlarm(false);
                this->pContext->setAlarm(true);
            }

            if (this->resetButton->isPressed())
            {
                this->setState(NORMAL);
            }
            break;
    }
}

void HangarTask::setState(State state)
{
    this->state = state;
    stateTimestamp = millis();
    justEntered = true;
}

long HangarTask::elapsedTimeInState() { return millis() - stateTimestamp; }

bool HangarTask::checkAndSetJustEntered()
{
    bool bak = justEntered;
    if (justEntered)
    {
        justEntered = false;
    }
    return bak;
}
