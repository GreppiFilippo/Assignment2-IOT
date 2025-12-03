#ifndef __DOOR_CONTROL_TASK__
#define __DOOR_CONTROL_TASK__

#include "kernel/Task.hpp"
#include "model/Context.hpp"
#include "devices/ServoMotor.hpp"
#include <Arduino.h>

class DoorControlTask : public Task {
    private:
        Context* context;
        ServoMotor* doorMotor;

        void setState(int state);
        long elapsedTimeInState();
        void log(const String& msg);
        
        bool checkAndSetJustEntered();
        
        enum { IDLE, STARTING, SWEEPING_FWD, SWEEPING_BWD, RESETTING } state;
        long stateTimestamp;
        bool justEntered;

        int currentPos;
        bool toBeStopped;

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
