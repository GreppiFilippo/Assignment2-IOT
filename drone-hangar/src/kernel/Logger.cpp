#include "Logger.hpp"

#include "MsgService.hpp"

void LoggerService::log(const String& msg)
{
    // Invece di creare una nuova String "lo:" + msg,
    // usiamo una funzione dedicata che non alloca memoria extra
    MsgService.sendMsgRaw("lo:", false);       // Stampa senza newline
    MsgService.sendMsgRaw(msg.c_str(), true);  // Stampa il contenuto e va a capo
}

void LoggerService::log(const __FlashStringHelper* msg)
{
    MsgService.sendMsgRaw("lo:", false);
    MsgService.sendMsgRaw(msg, true);
}