#include "Skedulone.hpp"

#include <Arduino.h>

void Skedulone::init(unsigned long basePeriod)
{
    this->basePeriod = basePeriod;
    this->nTasks = 0;
    this->lastTick = millis();
}

bool Skedulone::addTask(Task* task)
{
    if (nTasks >= MAX_TASKS)
        return false;

    taskList[nTasks++] = task;
    return true;
}

void Skedulone::schedule()
{
    unsigned long now = millis();

    if (now - lastTick < basePeriod)
        return;

    lastTick += basePeriod;

    for (int i = 0; i < nTasks; i++)
    {
        Task* t = taskList[i];

        if (!t->isActive())
            continue;

        if (t->isPeriodic())
        {
            if (t->updateAndCheckTime(basePeriod))
            {
                t->tick();
            }
        }
        else
        {
            t->tick();
            if (t->isCompleted())
            {
                t->setActive(false);
            }
        }
    }
}
