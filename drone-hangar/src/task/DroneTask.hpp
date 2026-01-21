#ifndef __HANGAR_TASK__
#define __HANGAR_TASK__

#include <Arduino.h>

#include "devices/ServoMotor.hpp"
#include "kernel/Task.hpp"
#include "kernel/MsgService.hpp"
#include "model/Context.hpp"
#include "devices/Led.hpp"
#include "task/BlinkingTask.hpp"

class DroneTask : public Task
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
        MsgServiceClass* msgService;

        Led* L1;
        BlinkingTask* L2;
        Led* L3;

        void setState(State state);
        long elapsedTimeInState();
        void log(const String& msg);
        bool checkAndSetJustEntered();
        bool receiveOpenCMD();
        //send ?

    public:
        DroneTask(Context* ctx);
        void tick();
};

#endif /* __HANGAR_TASK__ */