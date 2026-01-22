#include "task/MSGTask.hpp"

#include <ArduinoJson.h>

#include "config.hpp"
#include "kernel/Logger.hpp"

MsgTask::MsgTask(Context* pContext, MsgServiceClass* pMsgService)
{
    this->pContext = pContext;
    this->pMsgService = pMsgService;
    this->lastJsonSent = millis();
}

void MsgTask::tick()
{
    // 1. Clean expired incoming messages
    Logger.log("Checking for expired messages...");
    int removed = pContext->cleanExpiredMessages();
    Logger.log("Expired messages cleaned: " + String(removed));
    if (removed > 0)
    {
        Logger.log("Cleaned " + String(removed) + " expired message(s)");
    }

    // 2. Consume new messages from serial (INPUT)
    // Look for incoming messages
    if (pMsgService->isMsgAvailable())
    {
        msg = pMsgService->receiveMsg();
        if (msg != nullptr)
        {
            String content = msg->getContent();
            Logger.log("MSG received: " + content);
            // Parse JSON to extract "command" field
            // Use StaticJsonDocument to avoid deprecated DynamicJsonDocument
            // and to keep memory usage explicit on AVR platforms.
            StaticJsonDocument<256> doc;
            DeserializationError error = deserializeJson(doc, content);

            if (error)
            {
                Logger.log("WARNING: Invalid JSON received: " + String(error.c_str()));
            }
            else if (!doc["command"].isNull())
            {
                String command = doc["command"].as<String>();
                Logger.log("Command extracted: " + command);

                if (!pContext->addMessage(command))
                {
                    Logger.log("WARNING: Message queue full! Command lost: " + command);
                }
            }
            else
            {
                Logger.log("WARNING: JSON received without 'command' field");
            }

            // receiveMsg() allocates and transfers ownership, so free it.
            delete msg;
            msg = nullptr;
        }
    }

    // 3. Periodically send JSON state (OUTPUT)
    if (millis() - lastJsonSent >= JSON_UPDATE_PERIOD_MS)
    {
        String json = pContext->buildJSON();
        pMsgService->sendMsg(json);
        pContext->clearJsonFields();
        lastJsonSent = millis();
    }
}
