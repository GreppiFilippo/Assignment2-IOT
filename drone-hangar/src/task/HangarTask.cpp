#include "task/HangarTask.hpp"

#include "config.hpp"
#include "kernel/Logger.hpp"

HangarTask::HangarTask(TempSensor* tempSensor, Button* resetButton, Light* L3, Context* pContext)
    : tempSensor(tempSensor), resetButton(resetButton), L3(L3), pContext(pContext), state(NORMAL)
{
}

void HangarTask::tick()
{
    switch (state)
    {
        case NORMAL:
            if (checkAndSetJustEntered())
            {
                pContext->setPreAlarm(false);
                pContext->setAlarm(false);
                L3->switchOff();
                Logger.log(F("[HT] NORMAL"));
            }
            this->temperature = tempSensor->getTemperature();
            if (temperature >= TEMP1)
                setState(TRACKING_PRE_ALARM);
            break;

        case TRACKING_PRE_ALARM:
            if (checkAndSetJustEntered())
            {
                Logger.log(F("[HT] TRACKING PRE-ALARM"));
            }
            this->temperature = tempSensor->getTemperature();
            if (temperature < TEMP1)
                setState(NORMAL);
            else if (elapsedTimeInState() >= TIME3)
                setState(PREALARM);
            break;

        case PREALARM:
            if (checkAndSetJustEntered())
            {
                pContext->setPreAlarm(true);
                Logger.log(F("[HT] PREALARM ACTIVE"));
            }
            this->temperature = tempSensor->getTemperature();
            if (temperature < TEMP1)
                setState(NORMAL);
            else if (temperature >= TEMP2)
                setState(TRACKING_ALARM);
            break;

        case TRACKING_ALARM:
            if (checkAndSetJustEntered())
            {
                Logger.log(F("[HT] TRACKING ALARM"));
            }
            this->temperature = tempSensor->getTemperature();
            if (temperature < TEMP2)
                setState(PREALARM);
            else if (elapsedTimeInState() >= TIME4)
                setState(ALARM);
            break;

        case ALARM:
            if (checkAndSetJustEntered())
            {
                pContext->setPreAlarm(false);
                pContext->setAlarm(true);
                L3->switchOn();
                pContext->setLCDMessage(LCD_ALARM_STATE);
                Logger.log(F("[HT] ALARM"));
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
