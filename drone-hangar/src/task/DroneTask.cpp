#include "task/DroneTask.hpp"

#include "DroneTask.hpp"
#include "config.hpp"
#include "devices/Led.hpp"
#include "kernel/Logger.hpp"

/**
 * @brief Pattern to match OPEN_CMD messages.
 *
 */
class OpenPattern : public Pattern
{
   public:
    bool match(const Msg& m) override { return m.getContent().equals(OPEN_CMD); }
};

DroneTask::DroneTask(Context* pContext, Led* L1, Led* L3, PresenceSensor* presenceSensor)
{
    this->pContext = pContext;
    this->setState(REST);
    this->L1 = L1;
    this->L3 = L3;
    this->presenceSensor = presenceSensor;
}

void DroneTask::sendState(const String& state)
{
    this->pContext->setJsonField("drone_state", state);
}

void DroneTask::tick()
{
    switch (this->state)
    {
        case REST:
            Logger.log(F(DRONE_REST_STATE));
            this->sendState(DRONE_REST_STATE);

            if (checkAndSetJustEntered())
            {
                this->pContext->closeDoor();
                this->L1->switchOn();
                this->pContext->stopBlink();
                this->L3->switchOff();
            }
            this->pContext->setLCDMessage(this->pContext->isAlarmActive() ? LCD_ALARM_STATE
                                                                          : LCD_REST_STATE);

            if (receiveOpenCMD() && !(pContext->isPreAlarmActive() || pContext->isAlarmActive()))
            {
                setState(TAKING_OFF);
            }
            break;
        case TAKING_OFF:
            Logger.log(F(DRONE_TAKING_OFF_STATE));
            this->sendState(DRONE_TAKING_OFF_STATE);
            if (checkAndSetJustEntered())
            {
                pContext->openDoor();
                pContext->setLCDMessage(LCD_TAKING_OFF_STATE);
                pContext->blink();
            }

            if (pContext->isAlarmActive() && this->pContext->isDroneIn())
            {
                setState(REST);
            }

            if (!this->pContext->isDroneIn())
            {
                setState(OPERATING);
            }
            break;
        case OPERATING:
            Logger.log(F(DRONE_OPERATING_STATE));
            this->sendState(DRONE_OPERATING_STATE);
            if (this->checkAndSetJustEntered())
            {
                this->pContext->closeDoor();
                this->L1->switchOff();
                this->pContext->stopBlink();
                this->L3->switchOff();
            }

            this->pContext->setLCDMessage(this->pContext->isAlarmActive() ? LCD_ALARM_STATE
                                                                          : LCD_OPERATING_STATE);

            if (receiveOpenCMD() && !(pContext->isPreAlarmActive() || pContext->isAlarmActive()) &&
                this->presenceSensor->isDetected())
            {
                setState(LANDING);
            }
            break;
        case LANDING:
            Logger.log(F(DRONE_LANDING_STATE));
            this->sendState(DRONE_LANDING_STATE);

            if (this->checkAndSetJustEntered())
            {
                this->pContext->openDoor();
                this->pContext->setLCDMessage(LCD_LANDING_STATE);
                this->pContext->blink();
            }

            if (pContext->isAlarmActive() && !this->pContext->isDroneIn())
            {
                setState(OPERATING);
            }

            if (this->pContext->isDroneIn())
            {
                setState(REST);
            }
            break;
    }
}


void DroneTask::setState(State state)
{
    this->state = state;
    this->stateTimestamp = millis();
    this->justEntered = true;
}

long DroneTask::elapsedTimeInState() { return millis() - stateTimestamp; }

bool DroneTask::checkAndSetJustEntered()
{
    bool bak = justEntered;
    if (justEntered)
    {
        justEntered = false;
    }
    return bak;
}

bool DroneTask::receiveOpenCMD()
{
    OpenPattern pattern;
    // Returns true if message consumed, false otherwise
    return this->pContext->consumeMessage(pattern);
}
