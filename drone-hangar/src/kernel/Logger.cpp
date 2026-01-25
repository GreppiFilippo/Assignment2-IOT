#include "Logger.hpp"

#include "MsgService.hpp"

void LoggerService::log(const String& msg)
{
    static String tmp;
    tmp = "lo: ";
    tmp += msg;
    MsgService.sendMsg(tmp);
}