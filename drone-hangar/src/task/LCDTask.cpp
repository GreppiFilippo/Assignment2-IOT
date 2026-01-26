#include "task/LCDTask.hpp"

#include <string.h>

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

    if (this->lastMsg == nullptr || strcmp(msg, this->lastMsg) != 0)
    {
        this->lcd->print(msg);
        this->lastMsg = msg;
    }
}
