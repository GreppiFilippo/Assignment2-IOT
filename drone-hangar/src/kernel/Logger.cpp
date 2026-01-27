#include "Logger.hpp"

#include "MsgService.hpp"

/**
 * @brief Logs a message by prepending the "lo:" tag.
 * @param msg The message content.
 */
void LoggerService::log(const String& msg)
{
    String logMsg = "lo:";
    logMsg += msg;
    MsgService.sendMsg(logMsg);
}
