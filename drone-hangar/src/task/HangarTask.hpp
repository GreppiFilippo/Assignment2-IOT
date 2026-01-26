#ifndef __TEMP_ALARM_TASK__
#define __TEMP_ALARM_TASK__

#include <Arduino.h>

#include "devices/Button.hpp"
#include "devices/Light.hpp"
#include "devices/TempSensor.hpp"
#include "kernel/Task.hpp"
#include "model/Context.hpp"

class HangarTask : public Task
{
   private:
    TempSensor* tempSensor;
    Button* resetButton;
    Light* L3;
    Context* pContext;

    HangarState state;
    long stateTimestamp;
    bool justEntered;
    float temperature;
    unsigned long startTime;

    void setState(HangarState state);
    long elapsedTimeInState();
    void log(const String& msg);
    bool checkAndSetJustEntered();

   public:
    HangarTask(TempSensor* tempSensor, Button* resetButton, Light* L3, Context* pContext);
    void tick();
};

#endif /* __TEMP_ALARM_TASK__ */
