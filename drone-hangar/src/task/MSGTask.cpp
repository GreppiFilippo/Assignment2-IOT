#include "task/MSGTask.hpp"

#include <ArduinoJson.h>

#include "config.hpp"
#include "kernel/Logger.hpp"

// Use static/global JSON documents and buffer to avoid large stack usage on AVR
static StaticJsonDocument<128> _jsonIn;
static StaticJsonDocument<128> _jsonOut;
static char _jsonBuf[128];

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

        _jsonIn.clear();
        if (deserializeJson(_jsonIn, payload) == DeserializationError::Ok)
        {
            const char* cmd = _jsonIn["command"] | "";
            if (*cmd)
                pContext->tryEnqueueMsg(cmd);
        }

        delete msg;
        processed++;
    }

    if (millis() - lastJsonSent >= JSON_UPDATE_PERIOD_MS)
    {
        // Use global buffer to avoid stack allocation
        _jsonOut.clear();
        this->pContext->serializeData(_jsonOut);
        _jsonOut["alive"] = true;

        size_t len = serializeJson(_jsonOut, _jsonBuf, sizeof(_jsonBuf));
        if (len < sizeof(_jsonBuf))
        {
            _jsonBuf[len] = '\0';
            pMsgService->sendMsg(_jsonBuf);
        }
        lastJsonSent = millis();
    }
}

