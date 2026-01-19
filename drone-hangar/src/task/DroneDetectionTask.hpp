#ifndef DRONE_DETECTION_TASK
#define DRONE_DETECTION_TASK

#include <Arduino.h>

#include "devices/Pir.hpp"
#include "devices/Sonar.hpp"
#include "kernel/Task.hpp"
#include "model/Context.hpp"

class DroneDetectionTask : public Task
{
   private:
    Sonar* sonarSensor;
    Context* pContext;
    bool pirPreviouslyActive;

    enum State
    {
        IDLE,
        LANDING_MONITORING,
        LANDING_WAITING,
        TAKEOFF_MONITORING,
        TAKEOFF_WAITING
    } state;

    void setState(State state);
    long elapsedTimeInState();
    void log(const String& msg);
    bool checkAndSetJustEntered();

   public:
    DroneDetectionTask(Pir* pirSensor, Sonar* sonarSensor, Context* pContext);
    void tick();
};

#endif /* DRONE_DETECTION_TASK */