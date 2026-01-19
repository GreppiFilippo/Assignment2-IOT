#ifndef __LOGGER__
#define __LOGGER__

#include "Arduino.h"

/**
 * @brief Service for logging messages.
 *
 */
class LoggerService
{
   public:
    /**
     * @brief Logs a message to the logging service.
     *
     * @param msg The message to log.
     */
    void log(const String& msg);
};

extern LoggerService Logger;

#endif
