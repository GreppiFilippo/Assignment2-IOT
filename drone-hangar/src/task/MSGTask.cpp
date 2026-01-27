#include "task/MSGTask.hpp"

#include <Arduino.h>
#include <string.h>

#include "config.hpp"
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
    const unsigned long now = millis();

    pContext->cleanupExpired(now);

    if (pMsgService->isMsgAvailable())
    {
        Msg* msg = pMsgService->receiveMsg();
        if (msg)
        {
            const String& content = msg->getContent();

            const int jsonPos = content.indexOf('{');
            if (jsonPos >= 0)
            {
                StaticJsonDocument<JSON_IN_SIZE> jsonIn;

                if (deserializeJson(jsonIn, content.c_str() + jsonPos) == DeserializationError::Ok)
                {
                    const char* cmd = jsonIn[COMMAND] | nullptr;
                    if (cmd)
                    {
                        pContext->tryEnqueueMsg(cmd);
                    }
                }
            }
        }
    }

    if (now - lastJsonSent >= JSON_UPDATE_PERIOD_MS)
    {
        StaticJsonDocument<JSON_OUT_SIZE> jsonOut;
        pContext->serializeData(jsonOut);
        jsonOut[ALIVE] = true;

        char jsonBuf[128];
        memset(jsonBuf, 0, sizeof(jsonBuf));
        serializeJson(jsonOut, jsonBuf, sizeof(jsonBuf));

        pMsgService->sendMsgRaw(jsonBuf, true);
        lastJsonSent = now;
    }
}