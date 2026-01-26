#include "Logger.hpp"

#include "MsgService.hpp"
#include <stdio.h>

void LoggerService::log(const String& msg)
{
    char buf[128];
    snprintf(buf, sizeof(buf), "lo:%s", msg.c_str());
    MsgService.sendMsg(buf);
}
