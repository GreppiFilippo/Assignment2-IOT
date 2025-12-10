#ifndef __TEMP_MONITOR_TASK__
#define __TEMP_MONITOR_TASK__


class TempMonitorTask: public Task {
    public:
        TempMonitorTask() {}
        void init(int period) {
            Task::init(period);
        }
    private:

};




#endif /* __TEMP_MONITOR_TASK__ */