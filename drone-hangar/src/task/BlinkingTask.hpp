#ifndef __BLINKING_TASK__
#define __BLINKING_TASK__

#include <Arduino.h>

#include "devices/Led.hpp"
#include "kernel/Task.hpp"
#include "model/Context.hpp"

class BlinkingTask : public Task
{
   public:
    BlinkingTask(Led* pLed, Context* pContext);
    void tick();

   private:
    enum State
    {
        IDLE,
        OFF,
        ON
    } state;
    long stateTimestamp;
    bool justEntered;

    Led* pLed;
    Context* pContext;

    void setState(State s);
    long elapsedTimeInState();
    void log(const String& msg);

    bool checkAndSetJustEntered();
};

#endif /* __BLINKING_TASK__ */