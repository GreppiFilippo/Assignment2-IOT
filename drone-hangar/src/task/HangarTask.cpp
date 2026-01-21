#include "task/HangarTask.hpp"

#include "config.hpp"
#include "kernel/Logger.hpp"

HangarTask::HangarTask(TempSensor* tempSensor, Button* resetButton, Context* pContext)
    : pContext(pContext), tempSensor(tempSensor), resetButton(resetButton)
{
    setState(NORMAL);
}

void HangarTask::tick()
{
    this->temperature = tempSensor->getTemperature();
    switch (state)
    {
        case NORMAL:
            if (this->checkAndSetJustEntered())
            {
                Logger.log(F("[TAT] NORMAL"));
            }

            if (this->temperature >= TEMP1)
            {
                this->setState(TRACKING_PRE_ALARM);
            }
            break;
        case TRACKING_PRE_ALARM:
            if (this->checkAndSetJustEntered())
            {
                Logger.log(F("[TAT] TRACKING_PRE_ALARM"));
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
            if (this->checkAndSetJustEntered())
            {
                Logger.log(F("[TAT] PREALARM"));
                this->pContext->setPreAlarm(true);
            }

            if (this->temperature < TEMP1)
            {
                this->pContext->setPreAlarm(false);
                this->setState(NORMAL);
            }
            else if (this->temperature >= TEMP2)
            {
                this->setState(TRACKING_ALARM);
            }
            break;
        case TRACKING_ALARM:
            if (this->checkAndSetJustEntered())
            {
                Logger.log(F("[TAT] TRACKING_ALARM"));
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
            if (this->checkAndSetJustEntered())
            {
                Logger.log(F("[TAT] ALARM"));
                this->pContext->setAlarm(true);
            }

            if (this->resetButton->isPressed())
            {
                this->pContext->setAlarm(false);
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
