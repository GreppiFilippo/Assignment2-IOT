#include "task/DroneTask.hpp"
#include "devices/Led.hpp"
#include "config.hpp"

DroneTask::DroneTask(Context* pContext){
    this->pContext = pContext;
    setState(DRONE_INSIDE);
    this->L1 = new Led(L1_PIN);
    this->L1 = new Led(L2_PIN);
}

void DroneTask::tick(){
    switch (this->state)
    {
    case DRONE_INSIDE:
        if(checkAndSetJustEntered()){
            pContext->closeDoor();
            L1->switchOn();
            pContext->stopBlink();
            L3->switchOff();
        }
        pContext->setLCDMessage(pContext->isAlarmActive() ? ALARM_MSG : IN_MSG);
        if(receiveOpenCMD() && !(pContext->isPreAlarmActive() || pContext->isAlarmActive())){
            setState(TAKE_OFF);
        }
        break;
    case TAKE_OFF:
        if(checkAndSetJustEntered()) {
            pContext->openDoor();
            pContext->setLCDMessage(TAKE_OFF_MGS);
            pContext->blink();
        }
        if(pContext->isAlarmActive()) {
            setState(DRONE_INSIDE);
        }
        break;
    case DRONE_OUTSIDE:

        break;
    case LANDING:

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

bool DroneTask::receiveOpenCMD() {
    OpenPattern pattern;
    if(this->msgService->isMsgAvailable(pattern)) {
        this->msgService->receiveMsg(pattern);
        return true;
    }
    return false;
}

class OpenPattern : public Pattern {
public:
    bool match(const Msg& m) override {
        return m.getContent().equals(OPEN_CMD);
    }
};
