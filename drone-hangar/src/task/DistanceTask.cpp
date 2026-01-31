#include "DistanceTask.hpp"

#include <Arduino.h>

#include "config.hpp"
#include "kernel/Logger.hpp"

DistanceTask::DistanceTask(ProximitySensor* sonarSensor, Context* pContext)
{
    this->sonarSensor = sonarSensor;
    this->pContext = pContext;
    this->setState(IDLE);
}

void DistanceTask::tick()
{
    switch (state)
    {
        case IDLE:
            if (checkAndSetJustEntered())
            {
                Logger.log(F("[DISTANCE] IDLE"));
                this->pContext->setDistance(-1);  // Indicate no reading
            }
            if (this->pContext->landingCheckRequested())
            {
                setState(LANDING_MONITORING);
            }
            if (this->pContext->takeoffCheckRequested())
            {
                setState(TAKEOFF_MONITORING);
            }
            break;

        case LANDING_MONITORING:
            if (checkAndSetJustEntered())
            {
                Logger.log(F("[DISTANCE] LANDING MONITORING"));
            }
            distance = sonarSensor->getDistance();
            this->pContext->setDistance(distance);
            if (distance <= D2)
            {
                setState(LANDING_WAITING);
            }
            if (!pContext->landingCheckRequested())
            {
                setState(IDLE);
            }
            break;

        case LANDING_WAITING:
            if (checkAndSetJustEntered())
            {
                Logger.log(F("[DISTANCE] LANDING WAITING"));
            }
            distance = sonarSensor->getDistance();
            this->pContext->setDistance(distance);
            if (distance > D2)
            {
                setState(LANDING_MONITORING);
            }

            if (this->elapsedTimeInState() > TIME2 || !pContext->landingCheckRequested())
            {
                this->pContext->setDroneIn(true);
                this->pContext->closeLandingCheck();
                setState(IDLE);
            }
            break;

        case TAKEOFF_MONITORING:
            if (checkAndSetJustEntered())
            {
                Logger.log(F("[DISTANCE] TAKEOFF MONITORING"));
            }
            distance = sonarSensor->getDistance();
            this->pContext->setDistance(distance);
            if (distance >= D1)
            {
                setState(TAKEOFF_WAITING);
            }
            if (!pContext->takeoffCheckRequested())
            {
                setState(IDLE);
            }
            break;

        case TAKEOFF_WAITING:
            if (checkAndSetJustEntered())
            {
                Logger.log(F("[DISTANCE] TAKEOFF WAITING"));
            }
            distance = sonarSensor->getDistance();
            this->pContext->setDistance(distance);
            if (distance < D1)
            {
                setState(TAKEOFF_MONITORING);
            }
            if (this->elapsedTimeInState() > TIME1 || !pContext->takeoffCheckRequested())
            {
                this->pContext->setDroneIn(false);
                this->pContext->closeTakeoffCheck();
                setState(IDLE);
            }
            break;
    }
}

void DistanceTask::setState(State state)
{
    this->state = state;
    this->stateTimestamp = millis();
    this->justEntered = true;
}

long DistanceTask::elapsedTimeInState() { return millis() - stateTimestamp; }

bool DistanceTask::checkAndSetJustEntered()
{
    bool bak = justEntered;
    if (justEntered)
    {
        justEntered = false;
    }
    return bak;
}
