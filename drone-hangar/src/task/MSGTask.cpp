#include "task/MSGTask.hpp"

#include <ArduinoJson.h>

#include "config.hpp"
#include "kernel/Logger.hpp"

MsgTask::MsgTask(Context* pContext, MsgServiceClass* pMsgService)
{
    this->pContext = pContext;
    this->pMsgService = pMsgService;
    this->lastJsonSent = millis();
    this->msg = nullptr;
}

void MsgTask::tick()
{
    // Cleanup expired queued commands before processing new messages
    pContext->cleanupExpired(millis());

    /* Process new messages from serial */
    while (pMsgService->isMsgAvailable())
    {
        msg = pMsgService->receiveMsg();
        if (!msg)
            continue;

        String content = msg->getContent();
        Logger.log("MSG received: " + content);

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, content);

        if (error)
        {
            Logger.log("WARNING: Invalid JSON received: " + String(error.c_str()));
        }
        else if (doc["command"].is<const char*>())
        {
            String commandStr = doc["command"].as<String>();
            if (pContext->tryEnqueueMsg(commandStr))
                Logger.log("Command recognized and enqueued: " + commandStr);
            else
                Logger.log("Command not recognized, ignored: " + commandStr);
        }

        // free Msg allocated by MsgService to avoid memory leak
        delete msg;
        msg = nullptr;
    }

    /* Periodically send JSON state (OUTPUT) */
    if (millis() - lastJsonSent >= JSON_UPDATE_PERIOD_MS)
    {
        String json = pContext->buildJSON();
        pMsgService->sendMsg(json);
        pContext->clearJsonFields();
        lastJsonSent = millis();
    }
}