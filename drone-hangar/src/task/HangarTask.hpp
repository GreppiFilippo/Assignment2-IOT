#ifndef __HANGAR_TASK__
#define __HANGAR_TASK__

#include "kernel/Task.hpp"
#include "model/Context.hpp"
#include "devices/ServoMotor.hpp"
#include <Arduino.hpp>

class HangarTask : public Task {
    private:
        Context* pContext;

        enum { DRONE_INSIDE, TAKE_OFF, DRONE_OUTSIDE, LANDING } state;

    public:
        HangarTask(Context* ctx);
        void tick();
};



#endif /* __HANGAR_TASK__ */