#include "Logger.hpp"

#include "MsgService.hpp"

void LoggerService::log(const String& msg)
{
    static char buf[128];
    buf[0] = '\0';
    strncpy(buf, "lo: ", sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    size_t remaining = sizeof(buf) - strlen(buf) - 1;
    if (remaining > 0)
    {
        strncat(buf, msg.c_str(), remaining);
    }
    MsgService.sendMsg(buf);
}