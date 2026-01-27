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
    // 1. Cleanup expired commands from Context
    this->pContext->cleanupExpired(millis());

    // 2. Process Incoming Messages
    if (this->pMsgService->isMsgAvailable())
    {
        Msg* msg = this->pMsgService->receiveMsg();
        if (msg)
        {
            this->jsonIn.clear();
            // Parse JSON from message content
            DeserializationError error = deserializeJson(this->jsonIn, msg->getContent());

            if (!error)
            {
                // Ensure the COMMAND key exists and is a valid string
                if (this->jsonIn.containsKey(COMMAND) && this->jsonIn[COMMAND].is<const char*>())
                {
                    const char* cmd = this->jsonIn[COMMAND];
                    if (this->pContext->tryEnqueueMsg(cmd))
                    {
                        Logger.log(F("Command enqueued"));
                    }
                    else
                    {
                        Logger.log(F("Unknown command"));
                    }
                }
            }
            else
            {
                Logger.log(F("JSON Deserialization failed"));
            }
            delete msg;  // Prevent memory leaks
        }
    }

    // 3. Periodic Status Update (Heartbeat)
    if (millis() - lastJsonSent >= JSON_UPDATE_PERIOD_MS)
    {
        this->jsonOut.clear();
        // Populate JSON with Context data
        this->pContext->serializeData(this->jsonOut);
        this->jsonOut[ALIVE] = true;

        // Serialize output to shared buffer
        size_t len = serializeJson(this->jsonOut, this->jsonBuf, sizeof(this->jsonBuf));
        if (len < sizeof(this->jsonBuf))
        {
            this->pMsgService->sendMsg(this->jsonBuf);
        }
        else
        {
            Logger.log(F("Output buffer overflow"));
        }
        this->lastJsonSent = millis();
    }
}