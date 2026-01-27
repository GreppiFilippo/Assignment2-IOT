#ifndef __MSGSERVICE__
#define __MSGSERVICE__

#include <Arduino.h>

/** * @brief Max number of messages stored in the internal queue.
 */
#define MSG_SERVICE_QUEUE_SIZE 4

/**
 * @class Msg
 * @brief Represents a single message container.
 */
class Msg
{
   private:
    String content;

   public:
    Msg() : content("") {}
    Msg(const char* c) : content(c) {}
    const String& getContent() const { return content; }
};

class MsgServiceClass
{
   private:
    Msg queue[MSG_SERVICE_QUEUE_SIZE];
    int8_t qHead, qTail, qCount;

   public:
    /** @brief Initializes the message service with the specified baud rate. */
    void init(unsigned long baudRate);
    bool isMsgAvailable();
    Msg* receiveMsg();

    /** @brief Sends a string message. */
    void sendMsg(const String& msg);

    /** @brief Sends a message using Flash memory (F()) to save RAM. */
    void sendMsg(const __FlashStringHelper* msg);

    /** @brief Internal use only: adds a message to the queue. */
    bool enqueueMsg(const char* content);
};

extern MsgServiceClass MsgService;

#endif