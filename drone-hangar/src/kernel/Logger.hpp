#ifndef __LOGGER__
#define __LOGGER__

#include <Arduino.h>

/**
 * @brief Service for logging messages using fixed buffers.
 */
class LoggerService
{
   public:
    /**
     * @brief Logs a message to the logging service.
     * @param msg The message to log (C-string).
     */
    void log(const char* msg);

    /**
     * @brief Logs a String message (for compatibility).
     * @param msg The String to log.
     */
    void log(const String& msg) { log(msg.c_str()); }

    /**
     * @brief Logs a flash string message.
     * @param msg The flash string to log.
     */
    void log(const __FlashStringHelper* msg);
};

extern LoggerService Logger;

#endif
