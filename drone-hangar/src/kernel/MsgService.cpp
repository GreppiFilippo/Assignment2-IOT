#include "MsgService.hpp"

#include <Arduino.h>

// Internal buffer for serial input
static String content;

MsgServiceClass MsgService;

// === PUBLIC API ===

bool MsgServiceClass::isMsgAvailable() { return qCount > 0; }

Msg* MsgServiceClass::receiveMsg()
{
    if (qCount == 0)
        return nullptr;

    Msg* msg = queue[qHead];
    queue[qHead] = nullptr;
    qHead = (qHead + 1) % MSG_SERVICE_QUEUE_SIZE;
    qCount--;
    return msg;
}

void MsgServiceClass::init()
{
    Serial.begin(115200);

    content.reserve(256);
    content = "";

    // initialize queue
    for (int i = 0; i < MSG_SERVICE_QUEUE_SIZE; i++) queue[i] = nullptr;
    qHead = qTail = qCount = 0;
}

void MsgServiceClass::sendMsg(const String& msg) { Serial.println(msg); }

void MsgServiceClass::sendMsg(const char* msg) { Serial.println(msg); }

bool MsgServiceClass::enqueueMsg(Msg* msg)
{
    if (qCount >= MSG_SERVICE_QUEUE_SIZE)
        return false;
    queue[qTail] = msg;
    qTail = (qTail + 1) % MSG_SERVICE_QUEUE_SIZE;
    qCount++;
    return true;
}

bool MsgServiceClass::isMsgAvailable(Pattern& pattern)
{
    for (int i = 0; i < qCount; i++)
    {
        int idx = (qHead + i) % MSG_SERVICE_QUEUE_SIZE;
        if (queue[idx] && pattern.match(*queue[idx]))
            return true;
    }
    return false;
}

Msg* MsgServiceClass::receiveMsg(Pattern& pattern)
{
    for (int i = 0; i < qCount; i++)
    {
        int idx = (qHead + i) % MSG_SERVICE_QUEUE_SIZE;
        if (queue[idx] && pattern.match(*queue[idx]))
        {
            Msg* found = queue[idx];

            // shift elements after idx one position toward head
            for (int k = 0; k < qCount - i - 1; k++)
            {
                int from = (idx + 1 + k) % MSG_SERVICE_QUEUE_SIZE;
                int to = (idx + k) % MSG_SERVICE_QUEUE_SIZE;
                queue[to] = queue[from];
            }

            int last = (qHead + qCount - 1) % MSG_SERVICE_QUEUE_SIZE;
            queue[last] = nullptr;
            qCount--;
            qTail = (qHead + qCount) % MSG_SERVICE_QUEUE_SIZE;

            return found;
        }
    }
    return nullptr;
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
            // Ignore empty lines, try to enqueue the message
            if (content.length() > 0)
            {
                Msg* m = new Msg(content);
                if (!MsgService.enqueueMsg(m))
                {
                    // queue full: drop message and free memory
                    delete m;
                }
            }
            content = "";  // RESET BUFFER
        }
        else if (ch != '\r')
        {
            content += ch;
        }
    }
}