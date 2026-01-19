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
        unsigned long stateTimestamp;
        bool justEntered;
        int currentPos;
        enum { CLOSED, OPENING, OPEN, CLOSING } state;

        void setState(int state);
        unsigned long elapsedTimeInState();
        void log(const String& msg);
        bool checkAndSetJustEntered();

    public:
        DoorControlTask(Context* ctx, ServoMotor* motor){
            this->context = ctx;
            this->doorMotor = motor;
        }

        void init(unsigned long period) override {
            Task::init(period);
        }

        void tick() override {
            switch (this->state)
            {
            case CLOSED:
                if (!checkAndSetJustEntered()){
                    this->doorMotor->off();
                    this->justEntered = true;
                    this->setActive(false);
                }
                this->currentPos = 0;
                this->doorMotor->setPosition(this->currentPos);
                this->setState(OPENING);
                break;
            case OPENING:
            
                break;
            case OPEN:
                break;
            case CLOSING:
                break;
            }
        }
};

bool DoorControlTask::checkAndSetJustEntered(){
    bool bak = justEntered;
    if (justEntered){
      justEntered = false;
    }
    return bak;
}




#endif
