#include <task/HangarTask.hpp>

HangarTask::HangarTask(Context* pContext){
    this->pContext = pContext;
    setState(DRONE_INSIDE);
}

void HangarTask::tick(){
    switch (this->state)
    {
    case DRONE_INSIDE:
        if(checkAndSetJustEntered()){

        }
        
        if(!(pContext->isPreAlarmActive() || pContext->isAlarmActive())){
            setState(TAKE_OFF);
        }
        break;
    case TAKE_OFF:
        if(checkAndSetJustEntered()) {

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