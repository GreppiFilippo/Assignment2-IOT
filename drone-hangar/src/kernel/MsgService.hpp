#ifndef __MSGSERVICE__
#define __MSGSERVICE__

#include <Arduino.h>

/**
 * @brief Class representing a message with fixed-size content.
 */
class Msg
{
   private:
    char content[128];  // Reduced fixed-size buffer for message content
    size_t length;      // Actual length of the content

   public:
    /**
     * @brief Default constructor for Msg.
     */
    Msg() : length(0) { content[0] = '\0'; }

    /**
     * @brief Construct a new Msg object with fixed content.
     * @param str The string to copy into content (truncated if too long).
     */
    Msg(const char* str)
    {
        length = strlen(str);
        if (length >= sizeof(content))
            length = sizeof(content) - 1;
        memcpy(content, str, length);
        content[length] = '\0';
    }

    /**
     * @brief Get the message content as a C-string.
     * @return const char* Pointer to the content.
     */
    const char* getContent() const { return content; }

    /**
     * @brief Get the length of the content.
     * @return size_t Length.
     */
    size_t getLength() const { return length; }
};

/**
 * @brief Abstract base class for message patterns.
 */
class Pattern
{
   public:
    /**
     * Check whether message `m` matches the pattern.
     * @param m reference to the message to test
     * @return true if the pattern matches, false otherwise
     */
    virtual bool match(const Msg& m) = 0;
};

/**
 * @brief Message service class for sending and receiving messages with a circular queue.
 */
class MsgServiceClass
{
   private:
    static const int MSG_SERVICE_QUEUE_SIZE = 4;  // Reduced queue size
    Msg queue[MSG_SERVICE_QUEUE_SIZE];            // Fixed array of Msg objects
    int qHead;
    int qTail;
    int qCount;

   public:
    /**
     * @brief Initialize the message service (call at startup).
     */
    void init();

    /**
     * @brief Check if at least one message is available.
     * @return true if a message is present, false otherwise
     */
    bool isMsgAvailable();

    /**
     * @brief Receive (consume) the current message and return it.
     * After calling this, the message is no longer considered available.
     * @return pointer to the received `Msg` (nullptr if no message)
     */
    Msg* receiveMsg();

    /**
     * @brief Check if there is a message that matches `pattern`.
     * Does not consume the message; only verifies presence.
     * @param pattern reference to the pattern used to test messages
     * @return true if a matching message exists
     */
    bool isMsgAvailable(Pattern& pattern);

    /**
     * Receive (consume) the first message that matches `pattern`.
     * Returns nullptr if no matching message is found.
     * @param pattern pattern used to filter messages
     * @return pointer to the received `Msg` or nullptr
     */
    Msg* receiveMsg(Pattern& pattern);

    /**
     * @brief Send a new message into the service.
     * @param msg the message text to send (C-string)
     */
    void sendMsg(const char* msg);

    /**
     * @brief Enqueue an already-constructed Msg into the receive queue.
     * Returns true if enqueued, false if queue was full.
     * @param msg pointer to the Msg to enqueue
     * @return true if enqueued, false otherwise
     */
    bool enqueueMsg(const char* content);
};

extern MsgServiceClass MsgService;

#endif
