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

        const char* payload = msg->getContent().c_str();

        StaticJsonDocument<128> in;
        if (deserializeJson(in, payload) == DeserializationError::Ok)
        {
            const char* cmd = in["command"] | "";
            if (*cmd)
                pContext->tryEnqueueMsg(cmd);
        }

        delete msg;
        processed++;
    }

    if (millis() - lastJsonSent >= JSON_UPDATE_PERIOD_MS)
    {
        StaticJsonDocument<256> out;
        this->pContext->serializeData(out);
        out["alive"] = true;

        char buf[256];
        serializeJson(out, buf, sizeof(buf));
        pMsgService->sendMsg(buf);

        lastJsonSent = millis();
    }
}
