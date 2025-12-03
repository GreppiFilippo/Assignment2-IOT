#ifndef __DOOR_CONTROL_TASK__
#define __DOOR_CONTROL_TASK__

#include "kernel/Task.hpp"
#include "model/Context.hpp"
#include "devices/ServoMotor.hpp"

class DoorControlTask : public Task {
    private:
        Context* context;
        ServoMotor* doorMotor;

    public:
        DoorControlTask(Context* ctx, ServoMotor* motor){
            this->context = ctx;
            this->doorMotor = motor;
        }

        void init(unsigned long period) override {
            Task::init(period);
        }

        void tick() override {
            // Implement door control logic based on context
            if (context->droneIn) {
                
            } else if (context->droneOut) {
                // Close door logic
            }
        }
};




#endif
