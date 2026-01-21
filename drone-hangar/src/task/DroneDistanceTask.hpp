#ifndef DRONE_DETECTION_TASK
#define DRONE_DETECTION_TASK

#include <Arduino.h>

#include "devices/Sonar.hpp"
#include "kernel/Task.hpp"
#include "model/Context.hpp"

class DroneDistanceTask : public Task
{
   private:
    Sonar* sonarSensor;
    Context* pContext;
    float distance;

    long stateTimestamp;
    bool justEntered;

    enum State
    {
        IDLE,
        TAKEOFF_MONITORING,
        TAKEOFF_WAITING, 
        LANDING_MONITORING,
        LANDING_WAITING 
    } state;

    void setState(State state);
    long elapsedTimeInState();
    void log(const String& msg);
    bool checkAndSetJustEntered();
    
   public:
    DroneDistanceTask(Sonar* sonarSensor, Context* pContext);
    void tick();
};

#endif /* DRONE_DETECTION_TASK */