#include <task/AlarmTask.hpp>
#include "config.hpp"
#include "kernel/Logger.hpp"

AlarmTask::AlarmTask(TempSensorTMP36* tempSensor, Context* pContext): 
    pContext(pContext), tempSensor(tempSensor){
    setState(NORMAL);
}

void AlarmTask::init(int period) {

}

void AlarmTask::tick() {
    switch(state) {
        case NORMAL:
            if (this->checkAndSetJustEntered()){
                Logger.log(F("[AT] NORMAL"));
            }

            break;

        case WAITING_PREALARM:
            if (this->checkAndSetJustEntered()){
                Logger.log(F("[AT] WAITING_PREALARM"));
            }
        
            break;
        case PREALARM:
            if (this->checkAndSetJustEntered()){
                Logger.log(F("[AT] PREALARM"));
            }
        
            break;
        case WAITING_ALARM:
            if (this->checkAndSetJustEntered()){
                Logger.log(F("[AT] WAITING_ALARM"));
            }
        
            break;
        case ALARM:
            if (this->checkAndSetJustEntered()){
                Logger.log(F("[AT] ALARM"));
            }
            break;
    }
}

void AlarmTask::setState(int s) {
    state = s;
    stateTimestamp = millis();
    justEntered = true;
}

long AlarmTask::elapsedTimeInState() {
    return millis() - stateTimestamp;
}

bool AlarmTask::checkAndSetJustEntered() {
    bool bak = justEntered;
    if (justEntered){
      justEntered = false;
    }
    return bak;
}
