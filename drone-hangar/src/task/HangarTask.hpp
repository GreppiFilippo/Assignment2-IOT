#ifndef __HANGAR_TASK__
#define __HANGAR_TASK__

#include <Arduino.h>

#include "devices/ServoMotor.hpp"
#include "kernel/Task.hpp"
#include "model/Context.hpp"

class HangarTask : public Task
{
    private:
    Context* pContext;

    enum State
    {
        DRONE_INSIDE,
        TAKE_OFF,
        DRONE_OUTSIDE,
        LANDING
    } state;

    long stateTimestamp;
    bool justEntered;

    void setState(State state);
    long elapsedTimeInState();
    void log(const String& msg);
    bool checkAndSetJustEntered();

   public:
    HangarTask(Context* ctx);
    void tick();
};

#endif /* __HANGAR_TASK__ */