#ifndef __HANGAR_TASK__
#define __HANGAR_TASK__

#include <Arduino.h>

#include "devices/Led.hpp"
#include "devices/PresenceSensor.hpp"
#include "devices/ServoMotor.hpp"
#include "kernel/MsgService.hpp"
#include "kernel/Task.hpp"
#include "model/Context.hpp"
#include "task/BlinkingTask.hpp"

/**
 * @brief Task that manages the drone operations.
 *
 */
class DroneTask : public Task
{
   private:
    Context* pContext;

    enum State
    {
        REST,
        TAKING_OFF,
        OPERATING,
        LANDING
    } state;

    long stateTimestamp;
    bool justEntered;
    MsgServiceClass* msgService;

    Led* L1;
    BlinkingTask* L2;
    Led* L3;
    PresenceSensor* presenceSensor;

    void setState(State state);
    long elapsedTimeInState();
    void log(const String& msg);
    bool checkAndSetJustEntered();
    bool receiveOpenCMD();
    void sendState(const String& state);

   public:
    DroneTask(Context* ctx, Led* L1, BlinkingTask* L2, Led* L3, PresenceSensor* presenceSensor,
              MsgServiceClass* msgService);
    void tick();
};

#endif /* __HANGAR_TASK__ */