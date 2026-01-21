#include "DistanceTask.hpp"

#include <Arduino.h>

#include "config.hpp"

void DistanceTask::tick()
{
    switch (state)
    {
        case IDLE:
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
            distance = sonarSensor->getDistance();
            this->pContext->setJsonField("distance", distance);
            if (distance <= D2)
            {
                setState(LANDING_WAITING);
            }
            break;
        case LANDING_WAITING:
            distance = sonarSensor->getDistance();
            this->pContext->setJsonField("distance", distance);
            if (distance > D2)
            {
                setState(LANDING_MONITORING);
            }
            if (this->elapsedTimeInState() > T2)
            {
                this->pContext->setDroneIn(true);
                this->pContext->closeLandingCheck();
                setState(IDLE);
            }
            break;
        case TAKEOFF_MONITORING:
            distance = sonarSensor->getDistance();
            this->pContext->setJsonField("distance", distance);
            if (distance >= D1)
            {
                setState(TAKEOFF_WAITING);
            }
            break;
        case TAKEOFF_WAITING:
            distance = sonarSensor->getDistance();
            this->pContext->setJsonField("distance", distance);
            if (distance < D2)
            {
                setState(TAKEOFF_MONITORING);
            }
            if (this->elapsedTimeInState() > T1)
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
