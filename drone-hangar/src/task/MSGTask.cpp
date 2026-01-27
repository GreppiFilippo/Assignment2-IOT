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
    // Pulisce i comandi scaduti nella coda del Context
    pContext->cleanupExpired(millis());

    // === INPUT: Ricezione messaggi (Comandi da PC/Dashboard) ===
    if (pMsgService->isMsgAvailable())
    {
        Msg* msg = pMsgService->receiveMsg();
        if (msg)
        {
            const char* content = msg->getContent();
            
            // Usiamo un documento locale temporaneo per il parsing
            StaticJsonDocument<128> doc;
            DeserializationError err = deserializeJson(doc, content);

            if (!err)
            {
                // Se il JSON ha una chiave "command", proviamo a metterlo in coda
                const char* cmd = doc["command"] | "";
                if (*cmd)
                {
                    if (pContext->tryEnqueueMsg(cmd)) {
                        Logger.log(("Cmd OK: "), cmd);
                    } else {
                        Logger.log(("Cmd Unknown: "), cmd);
                    }
                }
            }
            delete msg;
        }
    }

    // === OUTPUT: Invio stato del sistema (Telemetria) ===
    if (millis() - lastJsonSent >= JSON_UPDATE_PERIOD_MS)
    {
        // NON SERVONO PIÙ: clearJsonFields() e setJsonField()
        // buildJSON() ora crea il JSON al volo usando i dati freschi nel Context
        
        pContext->buildJSON(Serial);
        Serial.println(); // Terminatore di riga per il ricevente
        lastJsonSent = millis();
}
}