#include "Logger.hpp"
#include "MsgService.hpp"
#include <string.h> // Per le funzioni standard delle stringhe C

LoggerService Logger;

void LoggerService::log(const __FlashStringHelper* msg) {
    char buf[128]; 
    // Copia dalla Flash alla RAM aggiungendo il prefisso "lo: "
    snprintf_P(buf, sizeof(buf), PSTR("lo: %S"), msg); 
    MsgService.sendMsg(buf);
}

void LoggerService::log(const char* msg) {
    char buf[128];
    snprintf(buf, sizeof(buf), "lo: %s", msg);
    MsgService.sendMsg(buf);
}

void LoggerService::log(const char* prefix, const char* msg) {
    Serial.print(F("lo: "));
    Serial.print(prefix);
    Serial.println(msg);
}
