#include "Logger.hpp"

#include "MsgService.hpp"

void LoggerService::log(const String& msg)
{
    static char buf[128];
    snprintf(buf, sizeof(buf), "lo: %s", msg.c_str());
    MsgService.sendMsg(buf);
}