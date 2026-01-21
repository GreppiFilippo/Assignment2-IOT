#include "task/MSGTask.hpp"

#include "config.hpp"
#include "kernel/Logger.hpp"
#include <ArduinoJson.h>

MsgTask::MsgTask(Context* pContext, MsgServiceClass* pMsgService)
{
    this->pContext = pContext;
    this->pMsgService = pMsgService;
    this->lastJsonSent = 0;
}

void MsgTask::tick()
{
    // 1. Clean expired incoming messages
    int removed = pContext->cleanExpiredMessages();
    if (removed > 0)
    {
        Logger.log("Cleaned " + String(removed) + " expired message(s)");
    }

    // 2. Consume new messages from serial (INPUT)
    if (pMsgService->isMsgAvailable())
    {
        Msg* msg = pMsgService->receiveMsg();
        if (msg != nullptr)
        {
            String content = msg->getContent();
            Logger.log("MSG received: " + content);

            // Parse JSON to extract "command" field
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, content);
            
            if (error)
            {
                Logger.log("WARNING: Invalid JSON received: " + String(error.c_str()));
            }
            else if (doc.containsKey("command"))
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

            delete msg;
        }
    }

    // 3. Periodically send JSON state (OUTPUT)
    unsigned long now = millis();
    if (now - lastJsonSent >= JSON_UPDATE_PERIOD_MS)
    {
        String json = pContext->buildJSON();
        pMsgService->sendMsg(json);
        pContext->clearJsonFields();
        lastJsonSent = now;
    }
}
