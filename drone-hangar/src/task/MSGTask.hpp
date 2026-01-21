#ifndef __MSG_TASK__
#define __MSG_TASK__

#include <Arduino.h>

#include "kernel/MsgService.hpp"
#include "kernel/Task.hpp"
#include "model/Context.hpp"

/**
 * @brief Task that continuously consumes messages from serial
 * and stores them in Context's message queue.
 * Also periodically sends JSON state updates to serial.
 */
class MsgTask : public Task
{
   private:
    Context* pContext;
    MsgServiceClass* pMsgService;
    unsigned long lastJsonSent;

   public:
    MsgTask(Context* pContext, MsgServiceClass* pMsgService);
    void tick() override;
};

#endif