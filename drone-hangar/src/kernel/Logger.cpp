#include "Logger.hpp"

#include "MsgService.hpp"

LoggerService Logger;

void LoggerService::log(const char* msg)
{
    char buf[128];
    size_t len = snprintf(buf, sizeof(buf), "lo:%s", msg);
    if (len < sizeof(buf))
    {
        MsgService.sendMsg(buf);
    }
    else
    {
        // Truncated, send anyway
        MsgService.sendMsg(buf);
    }
}

void LoggerService::log(const __FlashStringHelper* msg)
{
    char buf[128];
    strcpy_P(buf, (PGM_P)msg);
    log(buf);
}