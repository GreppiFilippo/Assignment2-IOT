#include "MsgService.hpp"

#include <Arduino.h>

// Internal buffer for serial input (fixed size)
static char serialBuffer[128];
static size_t serialBufferIndex = 0;

MsgServiceClass MsgService;

// === PUBLIC API ===

void MsgServiceClass::init()
{
    Serial.begin(115200);
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

bool MsgServiceClass::isMsgAvailable(Pattern& pattern)
{
    for (int i = 0; i < qCount; i++)
    {
        int idx = (qHead + i) % MSG_SERVICE_QUEUE_SIZE;
        if (pattern.match(queue[idx]))
            return true;
    }
    return false;
}

Msg* MsgServiceClass::receiveMsg(Pattern& pattern)
{
    for (int i = 0; i < qCount; i++)
    {
        int idx = (qHead + i) % MSG_SERVICE_QUEUE_SIZE;
        if (pattern.match(queue[idx]))
        {
            Msg* found = &queue[idx];

            // Shift elements after idx one position toward head
            for (int k = 0; k < qCount - i - 1; k++)
            {
                int from = (idx + 1 + k) % MSG_SERVICE_QUEUE_SIZE;
                int to = (idx + k) % MSG_SERVICE_QUEUE_SIZE;
                queue[to] = queue[from];
            }

            // No need to set to nullptr since Msg is not a pointer
            qCount--;
            qTail = (qHead + qCount) % MSG_SERVICE_QUEUE_SIZE;

            return found;
        }
    }
    return nullptr;
}

void MsgServiceClass::sendMsg(const char* msg) { Serial.println(msg); }

bool MsgServiceClass::enqueueMsg(const char* content)
{
    if (qCount >= MSG_SERVICE_QUEUE_SIZE)
        return false;

    queue[qTail] = Msg(content);
    qTail = (qTail + 1) % MSG_SERVICE_QUEUE_SIZE;
    qCount++;
    return true;
}

// === SERIAL EVENT HANDLER ===
// Called automatically by Arduino core between loop() iterations
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
                if (!MsgService.enqueueMsg(serialBuffer))
                {
                    // Queue full: silently drop (could log, but avoid recursion)
                }
            }
            serialBufferIndex = 0;  // Reset buffer
        }
        else if (ch != '\r' && serialBufferIndex < sizeof(serialBuffer) - 1)
        {
            serialBuffer[serialBufferIndex++] = ch;
        }
    }
}