#include "task/MSGTask.hpp"

#include <ArduinoJson.h>

#include "config.hpp"
#include "kernel/Logger.hpp"

// Costruttore - DEFINITO UNA SOLA VOLTA
MsgTask::MsgTask(Context* pContext, MsgServiceClass* pMsgService)
{
    this->pContext = pContext;
    this->pMsgService = pMsgService;
    this->lastJsonSent = millis();
}

void MsgTask::tick()
{
    // 1. Pulizia comandi scaduti nel Context
    this->pContext->cleanupExpired(millis());

    // 2. Gestione messaggi in arrivo
    if (this->pMsgService->isMsgAvailable())
    {
        Msg* msg = this->pMsgService->receiveMsg();
        if (msg)
        {
            this->jsonIn.clear();
            DeserializationError error = deserializeJson(this->jsonIn, msg->getContent());

            if (!error)
            {
                if (this->jsonIn.containsKey(COMMAND))
                {
                    const char* cmd = this->jsonIn[COMMAND];
                    Logger.log(F("Received command: "));
                    Logger.log(cmd);
                    if (this->pContext->tryEnqueueMsg(cmd))
                    {
                        Logger.log(F("Command enqueued"));
                    }
                }
            }
            else
            {
                Logger.log(F("JSON Err: "));
                Logger.log(error.c_str());
            }
            // NOTA: Nessun 'delete msg' qui.
        }
    }

    // 3. Invio periodico dello stato (JSON)
    if (millis() - lastJsonSent >= JSON_UPDATE_PERIOD_MS)
    {
        this->jsonOut.clear();
        this->pContext->serializeData(this->jsonOut);
        this->jsonOut[ALIVE] = true;

        // Serializzazione nel buffer jsonBuf definito nell'header del Task
        serializeJson(this->jsonOut, this->jsonBuf, sizeof(this->jsonBuf));

        // Invio tramite il servizio
        this->pMsgService->sendMsgRaw(this->jsonBuf, true);

        this->lastJsonSent = millis();
    }
}