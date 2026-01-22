#include "task/LCDTask.hpp"

#include "config.hpp"

LCDTask::LCDTask(LCD* lcd, Context* pContext)
{
    this->lcd = lcd;
    this->pContext = pContext;
    this->lastMsg = nullptr;
}

void LCDTask::tick()
{
    const char* msg = pContext->getLCDMessage();

    if (msg != lastMsg)
    {
        lcd->clear();
        lcd->print(msg);
        lastMsg = msg;
    }
}
