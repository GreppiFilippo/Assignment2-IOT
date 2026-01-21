#include "task/BlinkingTask.hpp"

#include "config.hpp"
#include "kernel/Logger.hpp"

BlinkingTask::BlinkingTask(Led* pLed, Context* pContext) : pContext(pContext), pLed(pLed)
{
    setState(IDLE);
}

void BlinkingTask::tick()
{
    switch (state)
    {
        case IDLE:
        {
            if (this->checkAndSetJustEntered())
            {
                pLed->switchOff();
            }
            if (pContext->isBlinking())
            {
                setState(OFF);
            }
            break;
        }
        case OFF:
        {
            if (this->checkAndSetJustEntered())
            {
                pLed->switchOff();
            }
            if (!pContext->isBlinking())
            {
                setState(IDLE);
            }
            else
            {
                setState(ON);
            }
            break;
        }
        case ON:
        {
            if (this->checkAndSetJustEntered())
            {
                pLed->switchOn();
            }
            if (!pContext->isBlinking())
            {
                setState(IDLE);
            }
            else
            {
                setState(OFF);
            }
            break;
        }
    }
}

void BlinkingTask::setState(State s)
{
    state = s;
    stateTimestamp = millis();
    justEntered = true;
}

long BlinkingTask::elapsedTimeInState() { return millis() - stateTimestamp; }

bool BlinkingTask::checkAndSetJustEntered()
{
    bool bak = justEntered;
    if (justEntered)
    {
        justEntered = false;
    }
    return bak;
}
