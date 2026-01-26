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
    pContext->cleanupExpired(millis());

    uint8_t processed = 0;
    while (pMsgService->isMsgAvailable() && processed < 5)
    {
        Msg* msg = pMsgService->receiveMsg();
        if (!msg)
            break;

        const char* payload = msg->getContent();

        StaticJsonDocument<128> in;
        if (deserializeJson(in, payload) == DeserializationError::Ok)
        {
            const char* cmd = in["command"] | "";
            if (*cmd)
                pContext->tryEnqueueMsg(cmd);
        }

        // No delete since Msg is not dynamically allocated
        processed++;
    }

    if (millis() - lastJsonSent >= JSON_UPDATE_PERIOD_MS)
    {
        // Serialize into a stack buffer then send through MsgService
        StaticJsonDocument<128> out;
        this->pContext->serializeData(out);
        out["alive"] = true;

        char buf[128];
        size_t len = serializeJson(out, buf, sizeof(buf));
        if (len < sizeof(buf))
        {
            pMsgService->sendMsg(buf);
        }
        lastJsonSent = millis();
    }
}
