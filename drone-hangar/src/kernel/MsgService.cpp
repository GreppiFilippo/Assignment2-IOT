#include "MsgService.hpp"

#include <Arduino.h>

// Internal buffer for serial input
static String content;

MsgServiceClass MsgService;

// === PUBLIC API ===

bool MsgServiceClass::isMsgAvailable() { return msgAvailable; }

Msg* MsgServiceClass::receiveMsg()
{
    if (!msgAvailable)
        return nullptr;

    Msg* msg = currentMsg;
    currentMsg = nullptr;
    msgAvailable = false;
    return msg;
}

void MsgServiceClass::init()
{
    Serial.begin(115200);

    content.reserve(256);
    content = "";

    currentMsg = nullptr;
    msgAvailable = false;
}

void MsgServiceClass::sendMsg(const String& msg) { Serial.println(msg); }

// === SERIAL EVENT HANDLER ===
// Called automatically by Arduino core between loop() iterations
void serialEvent()
{
    while (Serial.available())
    {
        char ch = (char)Serial.read();

        if (ch == '\n')
        {
            // Ignore empty lines
            if (content.length() > 0 && !MsgService.msgAvailable)
            {
                MsgService.currentMsg = new Msg(content);
                MsgService.msgAvailable = true;
            }
            content = "";  // 🔥 RESET BUFFER
        }
        else if (ch != '\r')
        {
            content += ch;
        }
    }
}