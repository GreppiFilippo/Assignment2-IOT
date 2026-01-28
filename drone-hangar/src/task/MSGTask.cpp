#include "task/MSGTask.hpp"

#include <Arduino.h>

#include "config.hpp"
#include "kernel/Logger.hpp"
#include "kernel/MsgService.hpp"
#include "model/Context.hpp"

MsgTask::MsgTask(Context* pContext, MsgServiceClass* pMsgService)
{
    this->pContext = pContext;
    this->pMsgService = pMsgService;
    this->lastJsonSent = millis();
}

void MsgTask::tick()
{
    this->pContext->cleanupExpired(millis());

    if (this->pMsgService->isMsgAvailable())
    {
        Msg* msg = this->pMsgService->receiveMsg();
        if (msg)
        {
            const String& content = msg->getContent();
            if (content.length() > 0)
            {
                char buffer[64];
                if (content.length() < sizeof(buffer))
                {
                    strcpy(buffer, content.c_str());
                    char* jsonStart = strchr(buffer, '{');
                    if (jsonStart)
                    {
                        StaticJsonDocument<JSON_IN_SIZE> jsonIn;
                        DeserializationError err = deserializeJson(jsonIn, jsonStart);
                        if (err == DeserializationError::Ok)
                        {
                            if (jsonIn.overflowed())
                            {
                                Logger.log(F("JSON_OVR"));
                            }
                            const char* cmd = jsonIn[COMMAND];

                            // Fallback: se il lookup diretto fallisce, cerchiamo manualmente
                            if (!cmd)
                            {
                                for (JsonPair kv : jsonIn.as<JsonObject>())
                                {
                                    if (strcmp(kv.key().c_str(), COMMAND) == 0)
                                    {
                                        if (kv.value().is<const char*>())
                                            cmd = kv.value().as<const char*>();
                                        break;
                                    }
                                }
                            }

                            if (cmd)
                            {
                                bool result = this->pContext->tryEnqueueMsg(cmd);
                                Logger.log(result ? F("CMD_OK") : F("CMD_ERR"));
                            }
                            else
                            {
                                Logger.log(F("CMD_NULL"));
                            }
                        }
                        else
                        {
                            Logger.log(F("JSON_ERR"));
                        }
                    }
                }
                else
                {
                    Logger.log(F("MSG_OVR"));
                }
            }
        }
    }

    if (millis() - lastJsonSent >= JSON_UPDATE_PERIOD_MS)
    {
        StaticJsonDocument<JSON_OUT_SIZE> jsonOut;
        this->pContext->serializeData(jsonOut);
        jsonOut[ALIVE] = true;

        char jsonBuf[128];
        memset(jsonBuf, 0, sizeof(jsonBuf));
        serializeJson(jsonOut, jsonBuf, sizeof(jsonBuf));

        this->pMsgService->sendMsgRaw(jsonBuf, true);
        lastJsonSent = millis();
    }
}
