#include "task/HangarTask.hpp"
#include "devices/Led.hpp"
#include "config.hpp"

HangarTask::HangarTask(Context* pContext){
    this->pContext = pContext;
    setState(DRONE_INSIDE);
    this->L1 = new Led(L1_PIN);
    this->L1 = new Led(L2_PIN);
}

void HangarTask::tick(){
    switch (this->state)
    {
    case DRONE_INSIDE:
        if(checkAndSetJustEntered()){
            pContext->closeDoor();
            L1->switchOn();
            //L3 stop blink
            L3->switchOff();
        }
        pContext->setLCDMessage(pContext->isAlarmActive() ? ALARM_MSG : IN_MSG);
        if(!(pContext->isPreAlarmActive() || pContext->isAlarmActive())){
            setState(TAKE_OFF);
        }
        break;
    case TAKE_OFF:
        if(checkAndSetJustEntered()) {
            pContext->openDoor();
        }
        break;
    case DRONE_OUTSIDE:

        break;
    case LANDING:

        break;
    }
}

void HangarTask::setState(State state)
{
    this->state = state;
    this->stateTimestamp = millis();
    this->justEntered = true;
}

long HangarTask::elapsedTimeInState() { return millis() - stateTimestamp; }

bool HangarTask::checkAndSetJustEntered()
{
    bool bak = justEntered;
    if (justEntered)
    {
        justEntered = false;
    }
    return bak;
}

bool HangarTask::receiveOpenCMD() {
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
