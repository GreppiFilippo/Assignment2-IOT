#include "task/TempAlarmTask.hpp"

#include "config.hpp"
#include "kernel/Logger.hpp"

#define NORMAL_STATE_MSG "HANGAR_STATE:NORMAL"
#define ALARM_STATE_MSG "HANGAR_STATE:ALARM"

TempAlarmTask::TempAlarmTask(TempSensor* tempSensor, Button* resetButton, Context* pContext)
    : pContext(pContext), tempSensor(tempSensor), resetButton(resetButton)
{
    setState(NORMAL);
}

void TempAlarmTask::tick()
{
    this->temperature = tempSensor->getTemperature();
    switch (state)
    {
        case NORMAL:
            Logger.log(F(NORMAL_STATE_MSG));

            if (this->checkAndSetJustEntered())
            {
                Logger.log(F("[TAT] NORMAL"));
                this->pContext->setPreAlarm(false);
                this->pContext->setAlarm(false);
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
            Logger.log(F(NORMAL_STATE_MSG));

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
            Logger.log(F(NORMAL_STATE_MSG));

            if (this->checkAndSetJustEntered())
            {
                Logger.log(F("[TAT] PREALARM"));
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
            Logger.log(F(NORMAL_STATE_MSG));

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
                this->pContext->setPreAlarm(false);
                this->pContext->setAlarm(true);
            }
            Logger.log(F(ALARM_STATE_MSG));

            if (this->resetButton->isPressed())
            {
                this->setState(NORMAL);
            }
            break;
    }
}

void TempAlarmTask::setState(State state)
{
    this->state = state;
    stateTimestamp = millis();
    justEntered = true;
}

long TempAlarmTask::elapsedTimeInState() { return millis() - stateTimestamp; }

bool TempAlarmTask::checkAndSetJustEntered()
{
    bool bak = justEntered;
    if (justEntered)
    {
        justEntered = false;
    }
    return bak;
}
