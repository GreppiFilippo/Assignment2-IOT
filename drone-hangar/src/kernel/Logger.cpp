#include "Logger.hpp"

#include "MsgService.hpp"

void LoggerService::log(const String& msg) { MsgService.sendMsg(msg); }
