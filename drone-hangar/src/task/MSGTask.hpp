#ifndef __MSG_TASK__
#define __MSG_TASK__

#include <Arduino.h>

#include "kernel/MsgService.hpp"
#include "kernel/Task.hpp"
#include "model/Context.hpp"


class MsgTask : public Task
{
    private:
    Context* pContext;
    MsgService* pMsgService;
};

#endif