#include "Logger.hpp"

#include "MsgService.hpp"

void LoggerService::log(const String& msg) { MsgService.sendMsg("lo:" + msg); }
