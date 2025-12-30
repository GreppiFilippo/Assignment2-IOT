#ifndef __TEMP_ALARM_TASK__
#define __TEMP_ALARM_TASK__

#include <Arduino.h>

#include "devices/Button.hpp"
#include "devices/TempSensor.hpp"
#include "kernel/Task.hpp"
#include "model/Context.hpp"

class TempAlarmTask : public Task
{
   private:
    Context* pContext;
    TempSensor* tempSensor;
    Button* resetButton;

    long stateTimestamp;
    bool justEntered;
    float temperature;
    unsigned long startTime;

    enum State
    {
        NORMAL,
        TRACKING_PRE_ALARM,
        PREALARM,
        TRACKING_ALARM,
        ALARM
    } state;

    void setState(State state);
    long elapsedTimeInState();
    void log(const String& msg);
    bool checkAndSetJustEntered();

   public:
    TempAlarmTask(TempSensor* tempSensor, Button* resetButton, Context* pContext);
    void tick();
};

#endif /* __TEMP_ALARM_TASK__ */
