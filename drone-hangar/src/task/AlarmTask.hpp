#ifndef __ALARM_TASK__
#define __ALARM_TASK__

#include "kernel/Task.hpp"
#include "model/Context.hpp"
#include <Arduino.h>

class AlarmTask: public Task {
    private:
        Context* pContext;

    public:
        AlarmTask(Context* pContext);
        void init(int period);
        void tick();

}

#endif /* __ALARM_TASK__ */
