#ifndef __MSGSERVICE__
#define __MSGSERVICE__

#include <Arduino.h>

#define MSG_SERVICE_QUEUE_SIZE 4

class Msg
{
   private:
    String content;

   public:
    Msg() : content("") {}
    // Usiamo il passaggio per referenza per evitare copie inutili
    void setContent(const char* c) { content = c; }
    const String& getContent() const { return content; }
};

class MsgServiceClass
{
   private:
    Msg queue[MSG_SERVICE_QUEUE_SIZE];
    int8_t qHead, qTail, qCount;

   public:
    void init(unsigned long baudRate);
    bool isMsgAvailable();

    // Restituiamo il puntatore, ma chi lo riceve NON deve deallocarlo
    Msg* receiveMsg();

    // Invio messaggi standard (aggiungono \n)
    void sendMsg(const String& msg);
    void sendMsg(const __FlashStringHelper* msg);

    // Invio Raw (per il Logger o invii parziali)
    void sendMsgRaw(const char* msg, bool newline);
    void sendMsgRaw(const __FlashStringHelper* msg, bool newline);

    bool enqueueMsg(const char* content);
};

extern MsgServiceClass MsgService;

#endif