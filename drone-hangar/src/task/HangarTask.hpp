#ifndef __HANGAR_TASK__
#define __HANGAR_TASK__

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

    uint32_t stateTimestamp;
    bool justEntered;
    float temperature;

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
    bool checkAndSetJustEntered();

   public:
    HangarTask(TempSensor* tempSensor, Button* resetButton, Light* L3, Context* pContext);
    void tick() override;
};

#endif
