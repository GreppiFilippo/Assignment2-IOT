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

    DroneState state;
    long stateTimestamp;
    bool justEntered;
    Light* L1;
    PresenceSensor* presenceSensor;

    void setState(DroneState state);
    long elapsedTimeInState();
    bool checkAndSetJustEntered();
    bool receiveOpenCMD();

   public:
    /**
     * @brief Construct a new Drone Task object
     *
     * @param pContext Pointer to the shared context object
     * @param L1 Pointer to the Light device
     * @param presenceSensor Pointer to the Presence Sensor device
     */
    DroneTask(Context* pContext, Light* L1, PresenceSensor* presenceSensor);

    /**
     * @brief Task execution method called by the scheduler when the task runs.
     *
     */
    void tick() override;
};

#endif /* __HANGAR_TASK__ */