#include "task/HangarTask.hpp"

#include "config.hpp"
#include "kernel/Logger.hpp"

HangarTask::HangarTask(TempSensor* tempSensor, Button* resetButton, Light* L3, Context* pContext)
    : tempSensor(tempSensor), resetButton(resetButton), L3(L3), pContext(pContext), state(NORMAL)
{
}

void HangarTask::tick()
{
    this->temperature = tempSensor->getTemperature();
    this->pContext->setTemperature(this->temperature);

    switch (state)
    {
        case NORMAL:
            if (checkAndSetJustEntered())
            {
                pContext->setHangarState(NORMAL);  // TODO: whatch out for NORMAL value not in
                                                   // normal
                pContext->setPreAlarm(false);
                pContext->setAlarm(false);
                L3->switchOff();
                Logger.log(F("[HT] NORMAL"));
            }
            if (temperature >= TEMP1)
                setState(TRACKING_PRE_ALARM);
            break;

        case TRACKING_PRE_ALARM:
            if (checkAndSetJustEntered())
            {
                pContext->setHangarState(NORMAL);
                startTime = millis();
                Logger.log(F("[HT] TRACKING PRE-ALARM"));
            }
            if (temperature < TEMP1)
                setState(NORMAL);
            else if (millis() - startTime >= TIME3)
                setState(PREALARM);
            break;

        case PREALARM:
            if (checkAndSetJustEntered())
            {
                pContext->setHangarState(NORMAL);
                pContext->setPreAlarm(true);
                Logger.log(F("[HT] PREALARM ACTIVE"));
            }
            if (temperature < TEMP1)
                setState(NORMAL);
            else if (temperature >= TEMP2)
                setState(TRACKING_ALARM);
            break;

        case TRACKING_ALARM:
            if (checkAndSetJustEntered())
            {
                pContext->setHangarState(NORMAL);
                startTime = millis();
                Logger.log(F("[HT] TRACKING ALARM"));
            }
            if (temperature < TEMP2)
                setState(PREALARM);
            else if (millis() - startTime >= TIME4)
                setState(ALARM);
            break;

        case ALARM:
            if (checkAndSetJustEntered())
            {
                pContext->setHangarState(ALARM);
                pContext->setPreAlarm(false);
                pContext->setAlarm(true);
                L3->switchOn();
                Logger.log(F("[HT] ALARM STATE"));
            }
            if (resetButton->isPressed())
                setState(NORMAL);
            break;
    }
}

void HangarTask::setState(State newState)
{
    state = newState;
    stateTimestamp = millis();
    justEntered = true;
}

long HangarTask::elapsedTimeInState() { return millis() - stateTimestamp; }

bool HangarTask::checkAndSetJustEntered()
{
    bool bak = justEntered;
    if (justEntered)
        justEntered = false;
    return bak;
}