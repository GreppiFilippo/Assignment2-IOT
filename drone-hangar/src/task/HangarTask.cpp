#include "task/HangarTask.hpp"

#include "config.hpp"
#include "kernel/Logger.hpp"

HangarTask::HangarTask(TempSensor* tempSensor, Button* resetButton, Light* L3, Context* pContext)
    : tempSensor(tempSensor), resetButton(resetButton), L3(L3), pContext(pContext)
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
                Logger.log(F("[HT] NORMAL"));
                this->pContext->setHangarState(NORMAL);
                this->pContext->setPreAlarm(false);
                this->pContext->setAlarm(false);
                this->L3->switchOff();
            }

            if (this->temperature >= TEMP1)
            {
                this->setState(TRACKING_PRE_ALARM);
            }
            break;
        case TRACKING_PRE_ALARM:
            if (this->checkAndSetJustEntered())
            {
                this->pContext->setHangarState(NORMAL);

                Logger.log(F("[HT] TRACKING_PRE_ALARM"));
                this->startTime = millis();
            }

            if (this->temperature < TEMP1)
            {
                this->setState(NORMAL);
            }
            else if (millis() - this->startTime >= TIME3)
            {
                this->setState(PREALARM);
            }
            break;
        case PREALARM:
            if (this->checkAndSetJustEntered())
            {
                this->pContext->setHangarState(NORMAL);

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

            if (this->checkAndSetJustEntered())
            {
                this->pContext->setHangarState(NORMAL);

                Logger.log(F("[HT] TRACKING_ALARM"));
                this->startTime = millis();
            }

            if (this->temperature < TEMP2)
            {
                this->setState(PREALARM);
            }
            else if (millis() - this->startTime >= TIME4)
            {
                this->setState(ALARM);
            }
            break;
        case ALARM:
            if (this->checkAndSetJustEntered())
            {
                this->pContext->setHangarState(ALARM);

                Logger.log(F("[HT] ALARM"));
                this->pContext->setPreAlarm(false);
                this->pContext->setAlarm(true);
                this->L3->switchOn();
            }

            if (this->resetButton->isPressed())
            {
                this->setState(NORMAL);
            }
            break;
    }
}

void HangarTask::setState(HangarState state)
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
