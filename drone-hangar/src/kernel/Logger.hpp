#ifndef __LOGGER__
#define __LOGGER__

#include "Arduino.h"

/**
 * @brief Service for logging messages.
 */
class LoggerService {
public:
    void log(const char* msg);                   // Per stringhe in RAM e buffer
    void log(const __FlashStringHelper* msg);    // Per stringhe con F()
    void log(const char* prefix, const char* msg);
};
extern LoggerService Logger;

#endif