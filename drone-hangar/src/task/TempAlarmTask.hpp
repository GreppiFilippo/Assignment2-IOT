#ifndef __TEMP_ALARM_TASK__
#define __TEMP_ALARM_TASK__

#include "kernel/Task.hpp"
#include "model/Context.hpp"
#include <Arduino.hpp>
#include "devices/TempSensorTMP36.hpp"

class TempAlarmTask: public Task {
    private:
        Context* pContext;
        TempSensorTMP36* tempSensor;
        long stateTimestamp;
        bool justEntered;
        float temperature;
        unsigned long startTime;

        enum { NORMAL, TRACKING_PRE_ALARM, PREALARM, TRACKING_ALARM, ALARM } state;

        void setState(int state);
        long elapsedTimeInState();
        void log(const String& msg);
        bool checkAndSetJustEntered();
        
    public:
        TempAlarmTask(TempSensorTMP36* tempSensor, Context* pContext);
        void tick();
};

#endif /* __TEMP_ALARM_TASK__ */
