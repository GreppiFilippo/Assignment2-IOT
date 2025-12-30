#ifndef __TASK__
#define __TASK__

class Task
{
   private:
    unsigned long myPeriod;
    unsigned long timeElapsed;
    bool active;
    bool periodic;
    bool completed;

   public:
    Task() { this->active = false; }

    /* periodic */
    virtual void init(unsigned long period)
    {
        this->myPeriod = period;
        this->periodic = true;
        this->active = true;
        this->timeElapsed = 0;
    }

    /* aperiodic */
    virtual void init()
    {
        this->timeElapsed = 0;
        this->periodic = false;
        this->active = true;
        this->completed = false;
    }

    virtual void tick() = 0;

    bool updateAndCheckTime(unsigned long basePeriod)
    {
        this->timeElapsed += basePeriod;
        if (this->timeElapsed >= this->myPeriod)
        {
            this->timeElapsed = 0;
            return true;
        }
        else
        {
            return false;
        }
    }

    void setCompleted()
    {
        this->completed = true;
        this->active = false;
    }

    bool isCompleted() { return this->completed; }

    bool isPeriodic() { return this->periodic; }

    bool isActive() { return this->active; }

    unsigned long getPeriod() { return this->myPeriod; }

    virtual void setActive(bool active)
    {
        timeElapsed = 0;
        this->active = active;
    }
};

#endif /* _TASK_ */
