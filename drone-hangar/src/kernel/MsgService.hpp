#ifndef __MSGSERVICE__
#define __MSGSERVICE__

#include "Arduino.h"

/**
 * @brief Class representing a message.
 *
 */
class Msg
{
   private:
    String content;

   public:
    /**
     * @brief Construct a new Msg object.
     *
     * @param content the message content
     */
    Msg(const String& content) : content(content) {}

    /**
     * @brief Get the message content.
     *
     * @return const String& reference to the message content
     */
    const String& getContent() const { return content; }
};

/**
 * @brief Abstract base class for message patterns.
 *
 */
class Pattern
{
   public:
    /**
     * Check whether message `m` matches the pattern.
     *
     * @param m reference to the message to test
     * @return true if the pattern matches, false otherwise
     */
    virtual bool match(const Msg& m) = 0;
};

/**
 * @brief Message service class for sending and receiving messages.
 *
 */
class MsgServiceClass
{
   public:
    // Single-slot removed in favor of a small circular queue
    static const int MSG_SERVICE_QUEUE_SIZE = 8;
    Msg* queue[MSG_SERVICE_QUEUE_SIZE];
    int qHead = 0;
    int qTail = 0;
    int qCount = 0;

   public:
    /**
     * Initialize the message service (call at startup).
     */
    void init();

    /**
     * Check if at least one message is available.
     *
     * @return true if a message is present, false otherwise
     */
    bool isMsgAvailable();

    /**
     * Receive (consume) the current message and return it.
     * After calling this, the message is no longer considered available.
     * @return pointer to the received `Msg` (nullptr if no message)
     */
    Msg* receiveMsg();

    /**
     * Check if there is a message that matches `pattern`.
     * Does not consume the message; only verifies presence.
     *
     * @param pattern reference to the pattern used to test messages
     * @return true if a matching message exists
     */
    bool isMsgAvailable(Pattern& pattern);

    /**
     * Receive (consume) the first message that matches `pattern`.
     * Returns nullptr if no matching message is found.
     *
     * @param pattern pattern used to filter messages
     * @return pointer to the received `Msg` or nullptr
     */
    Msg* receiveMsg(Pattern& pattern);

    /**
     * Send a new message into the service.
     *
     * @param msg the message text to send
     */
    void sendMsg(const String& msg);

    /**
     * Enqueue an already-constructed Msg into the receive queue.
     * Returns true if enqueued, false if queue was full.
     */
    bool enqueueMsg(Msg* msg);
};

extern MsgServiceClass MsgService;

#endif
