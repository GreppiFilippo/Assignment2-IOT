#ifndef __ALARM_TASK__
#define __ALARM_TASK__

#include "kernel/Task.hpp"
#include "model/Context.hpp"
#include <Arduino.hpp>
#include "devices/TempSensorTMP36.hpp"

class AlarmTask: public Task {
    private:
        Context* pContext;
        TempSensorTMP36* tempSensor;
        long stateTimestamp;
        bool justEntered;
        
        enum { NORMAL, WAITING_PREALARM, PREALARM, WAITING_ALARM, ALARM } state;

        void setState(int state);
        long elapsedTimeInState();
        void log(const String& msg);
        bool checkAndSetJustEntered();
        
    public:
        AlarmTask(TempSensorTMP36* tempSensor, Context* pContext);
        void init(int period);
        void tick();
};

#endif /* __ALARM_TASK__ */
