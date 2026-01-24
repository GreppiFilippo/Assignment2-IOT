
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
    this->pContext->cleanupExpired(millis());

    // Process new command messages from serial
    while (this->pMsgService->isMsgAvailable())
    {
        Msg* msg = this->pMsgService->receiveMsg();
        if (!msg)
        {
            continue;
        }

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
            const char* commandStr = doc["command"].as<const char*>();
            if (this->pContext->tryEnqueueMsg(commandStr))
                Logger.log("Command recognized and enqueued: " + String(commandStr));
            else
                Logger.log("Command not recognized, ignored: " + String(commandStr));
        }

        // free Msg allocated by MsgService to avoid memory leak
        delete msg;
        msg = nullptr;
    }

    // Periodically send JSON state (OUTPUT)
    if (millis() - this->lastJsonSent >= JSON_UPDATE_PERIOD_MS)
    {
        this->pContext->setJsonField("alive", true);
        String json = this->pContext->buildJSON();
        this->pMsgService->sendMsg(json);
        this->pContext->clearJsonFields();
        this->lastJsonSent = millis();
    }
}
