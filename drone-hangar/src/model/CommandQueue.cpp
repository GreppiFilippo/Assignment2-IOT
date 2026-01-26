#include "CommandQueue.hpp"

#include <Arduino.h>

#include "config.hpp"

CommandQueue::CommandQueue() : head(0), tail(0), count(0) {}

bool CommandQueue::enqueue(CommandType cmd, uint32_t now)
{
    bool success = false;
    noInterrupts();
    if (count < CMD_QUEUE_SIZE)
    {
        queue[tail].cmd = cmd;
        queue[tail].timestamp = now;
        tail = (tail + 1) % CMD_QUEUE_SIZE;
        count++;
        success = true;
    }
    interrupts();
    return success;
}

bool CommandQueue::consume(CommandType cmd)
{
    bool found = false;
    noInterrupts();
    int scanned = 0;
    while (scanned < count)
    {
        int index = (head + scanned) % CMD_QUEUE_SIZE;
        if (queue[index].cmd == cmd)
        {
            if (scanned != 0)
            {
                queue[index] = queue[head];
            }
            head = (head + 1) % CMD_QUEUE_SIZE;
            count--;
            found = true;
            break;
        }
        scanned++;
    }
    interrupts();
    return found;
}

void CommandQueue::cleanupExpired(uint32_t now)
{
    noInterrupts();
    int removed = 0;
    for (int i = 0; i < count; i++)
    {
        int index = (head + i) % CMD_QUEUE_SIZE;
        if ((now - queue[index].timestamp) >= CONFIG_CMD_TTL_MS)
        {
            removed++;
        }
        else if (removed > 0)
        {
            queue[(head + i - removed) % CMD_QUEUE_SIZE] = queue[index];
        }
    }
    head = (head + removed) % CMD_QUEUE_SIZE;
    count -= removed;
    interrupts();
}
