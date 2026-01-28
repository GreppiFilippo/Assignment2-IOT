#include "task/BlinkingTask.hpp"

#include "config.hpp"
#include "kernel/Logger.hpp"

BlinkingTask::BlinkingTask(Light* pLed, Context* pContext)
{
    this->pLed = pLed;
    this->pContext = pContext;
    setState(OFF);
}

void BlinkingTask::tick()
{
    switch (state)
    {
        case OFF:
        {
            if (this->checkAndSetJustEntered())
            {
                Logger.log(F("[BLINK] OFF"));
                pLed->switchOff();
            }
            if (pContext->isBlinking())
            {
                setState(ON);
            }
            break;
        }
        case ON:
        {
            if (this->checkAndSetJustEntered())
            {
                Logger.log(F("[BLINK] ON"));
                pLed->switchOn();
            }
            setState(OFF);
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
