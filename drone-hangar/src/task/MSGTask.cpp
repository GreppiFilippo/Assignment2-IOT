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
    // Cleanup expired commands
    pContext->cleanupExpired(millis());

    // === INPUT: receive ONE message per tick (safe) ===
    if (pMsgService->isMsgAvailable())
    {
        Msg* msg = pMsgService->receiveMsg();
        if (msg)
        {
            const String& content = msg->getContent();
            Logger.log("MSG received: " + content);

            JsonDocument doc;
            DeserializationError err = deserializeJson(doc, content);

            if (err)
            {
                Logger.log("WARNING: Invalid JSON received");
            }
            else
            {
                const char* cmd = doc["command"] | "";
                if (*cmd)
                {
                    if (pContext->tryEnqueueMsg(cmd))
                        Logger.log("Command enqueued: " + String(cmd));
                    else
                        Logger.log("Unknown command: " + String(cmd));
                }
            }

            delete msg;
        }
    }

    // === OUTPUT: periodic JSON state ===
    if (millis() - lastJsonSent >= JSON_UPDATE_PERIOD_MS)
    {
        pContext->clearJsonFields();
        pContext->setJsonField("alive", true);

        String json = pContext->buildJSON();
        pMsgService->sendMsg(json);

        lastJsonSent = millis();
    }
}