#ifndef __BLINKING_TASK__
#define __BLINKING_TASK__

#include <Arduino.h>

#include "devices/Led.hpp"
#include "kernel/Task.hpp"
#include "model/Context.hpp"

/**
 * @brief Task that manages the blinking of an LED based on the system context.
 *
 */
class BlinkingTask : public Task
{
    private:
        Light* pLed;
        Context* pContext;

        enum State
        {
            IDLE,
            OFF,
            ON
        } state;
        long stateTimestamp;
        bool justEntered;

        void setState(State s);
        long elapsedTimeInState();
        void log(const String& msg);

        bool checkAndSetJustEntered();

    public:
        BlinkingTask(Light* pLed, Context* pContext);
        void tick() override;
};

#endif /* __BLINKING_TASK__ */