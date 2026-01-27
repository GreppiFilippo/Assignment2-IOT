#include "MsgService.hpp"
#include <Arduino.h>

// Buffer interno per l'input seriale
#define MAX_MSG_SIZE 128
char msgBuffer[MAX_MSG_SIZE];
int msgIdx = 0;

MsgServiceClass MsgService;

// === PUBLIC API ===

bool MsgServiceClass::isMsgAvailable() { 
    return msgAvailable; 
}

Msg* MsgServiceClass::receiveMsg()
{
    if (!msgAvailable)
        return nullptr;

    Msg* msg = currentMsg;
    currentMsg = nullptr;
    msgAvailable = false;
    return msg;
}

void MsgServiceClass::init()
{
    Serial.begin(115200);
    
    // Reset dello stato
    msgIdx = 0;
    memset(msgBuffer, 0, MAX_MSG_SIZE); // Pulisce il buffer hardware
    
    currentMsg = nullptr;
    msgAvailable = false;
}

// Nota: se passi un JSON minificato, Serial.println lo invia correttamente
void MsgServiceClass::sendMsg(const char* msg) { 
    Serial.println(msg); 
}

// Gestione evento seriale (Hardware interrupt-like)
void serialEvent() 
{
    while (Serial.available()) 
    {
        char ch = (char)Serial.read();

        if (ch == '\n') // Fine del messaggio
        {
            if (msgIdx > 0 && !MsgService.isMsgAvailable()) 
            {
                msgBuffer[msgIdx] = '\0'; // Chiusura stringa C
                
                // Crea il messaggio. 
                // Assicurati che il costruttore di Msg faccia una COPIA di msgBuffer
                MsgService.currentMsg = new Msg(msgBuffer); 
                
                MsgService.msgAvailable = true;
            }
            msgIdx = 0; 
        } 
        else if (ch != '\r') // Ignora carriage return, salva il resto
        {
            if (msgIdx < MAX_MSG_SIZE - 1) 
            {
                msgBuffer[msgIdx++] = ch;
            }
        }
    }
}