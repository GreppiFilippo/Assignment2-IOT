#ifndef __SKEDULONE__
#define __SKEDULONE__

#include "Task.hpp"

#define MAX_TASKS 10

class Skedulone
{
   public:
    void init(unsigned long basePeriod);
    bool addTask(Task* task);
    void schedule();

   private:
    unsigned long basePeriod;
    int nTasks;
    Task* taskList[MAX_TASKS];
    unsigned long lastTick;
};

#endif
