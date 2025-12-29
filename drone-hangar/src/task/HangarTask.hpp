#ifndef __HANGAR_TASK__
#define __HANGAR_TASK__

#include "kernel/Task.hpp"
#include "model/Context.hpp"
#include "devices/ServoMotor.hpp"
#include <Arduino.hpp>

class HangarTask : public Task {
    private:
        Context* pContext;
        ServoMotor* pHangarMotor;

    public:
        HangarTask(Context* ctx, ServoMotor* motor) : pContext(ctx), pHangarMotor(motor) {}

        void init(unsigned long period) override {
            Task::init(period);
            pHangarMotor->on();
            // Initialize hangar motor position if needed
        }

        void tick() override {
            // Implement hangar-specific logic here
            // For example, monitor hangar status or control hangar mechanisms
        }
};



#endif /* __HANGAR_TASK__ */