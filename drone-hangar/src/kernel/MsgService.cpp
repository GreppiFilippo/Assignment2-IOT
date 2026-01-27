#include "MsgService.hpp"

// Static buffer for raw serial input to prevent fragmentation
static char serialBuffer[128];
static size_t serialBufferIndex = 0;

MsgServiceClass MsgService;

void MsgServiceClass::init(unsigned long baudRate)
{
    Serial.begin(baudRate);
    qHead = 0;
    qTail = 0;
    qCount = 0;
    serialBufferIndex = 0;
    serialBuffer[0] = '\0';
}

bool MsgServiceClass::isMsgAvailable() { return qCount > 0; }

Msg* MsgServiceClass::receiveMsg()
{
    if (qCount == 0)
        return nullptr;

    Msg* msg = &queue[qHead];
    qHead = (qHead + 1) % MSG_SERVICE_QUEUE_SIZE;
    qCount--;
    return msg;
}

void MsgServiceClass::sendMsg(const String& msg) { Serial.println(msg); }

void MsgServiceClass::sendMsg(const __FlashStringHelper* msg) { Serial.println(msg); }

bool MsgServiceClass::enqueueMsg(const char* content)
{
    if (qCount >= MSG_SERVICE_QUEUE_SIZE)
        return false;

    // The assignment operator of String inside Msg handles the copy
    queue[qTail] = Msg(content);
    qTail = (qTail + 1) % MSG_SERVICE_QUEUE_SIZE;
    qCount++;
    return true;
}

/**
 * @brief Standard Arduino serial event handler.
 * Reads characters and builds messages until a newline is found.
 */
void serialEvent()
{
    while (Serial.available())
    {
        char ch = (char)Serial.read();

        if (ch == '\n')
        {
            if (serialBufferIndex > 0)
            {
                serialBuffer[serialBufferIndex] = '\0';
                MsgService.enqueueMsg(serialBuffer);
            }
            serialBufferIndex = 0;
        }
        else if (ch != '\r' && serialBufferIndex < sizeof(serialBuffer) - 1)
        {
            serialBuffer[serialBufferIndex++] = ch;
        }
    }
}