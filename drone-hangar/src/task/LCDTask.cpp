#include "task/LCDTask.hpp"

#include "config.hpp"

LCDTask::LCDTask(LCD* lcd, Context* pContext)
{
    this->lcd = lcd;
    this->pContext = pContext;
    this->lastMsg = nullptr;  // Initialize lastMsg to nullptr to ensure first update
}

void LCDTask::tick()
{
    const char* msg = this->pContext->getLCDMessage();

    if (msg != this->lastMsg)
    {
        this->lcd->print(msg);
        this->lastMsg = msg;
    }
}
