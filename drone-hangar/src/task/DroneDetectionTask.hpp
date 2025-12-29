#ifndef DRONE_DETECTION_TASK
#define DRONE_DETECTION_TASK

#include "kernel/Task.hpp"
#include "model/Context.hpp"
#include "devices/Sonar.hpp"
#include "devices/Pir.hpp"
#include <Arduino.h>

class DroneDetectionTask : public Task {
    private:
        Sonar* sonarSensor;
        Context* pContext;
        bool pirPreviouslyActive;

        void setState(int state);
        long elapsedTimeInState();
        void log(const String& msg);
        bool checkAndSetJustEntered();

        enum { IDLE, LANDING_MONITORING, LANDING_WAITING, TAKEOFF_MONITORING, TAKEOFF_WAITING } state;

    public:
        DroneDetectionTask(Pir* pirSensor, Sonar* sonarSensor, Context* pContext);
        void tick();
};

#endif /* DRONE_DETECTION_TASK */