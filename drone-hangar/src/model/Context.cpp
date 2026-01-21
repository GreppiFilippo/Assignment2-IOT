#include "model/Context.hpp"

Context::Context()
{
    this->openDoorRequested = false;
    this->closeDoorRequested = false;
    this->doorOpen = false;
    this->alarmActive = false;
    this->preAlarmActive = false;
    this->currentDistance = 0;
    this->currentTemperature = 0;
    this->pirActive = false;
    this->lcdMessage = "";
    this->ledBlinking = false;

    // Initialize message queue
    this->queueHead = 0;
    this->queueTail = 0;
    this->queueCount = 0;
    for (int i = 0; i < MSG_QUEUE_SIZE; i++)
    {
        messageQueue[i].valid = false;
    }
}

void Context::closeDoor() { this->closeDoorRequested = true; }

bool Context::closeDoorReq() { return this->closeDoorRequested; }

void Context::openDoor() { this->openDoorRequested = true; }

bool Context::openDoorReq() { return this->openDoorRequested; }

bool Context::isDoorOpen() { return this->doorOpen; }

void Context::setDoorOpened() { this->doorOpen = true; }

void Context::setDoorClosed()
{
    this->closeDoorRequested = false;
    this->openDoorRequested = false;
    this->doorOpen = false;
}

void Context::setAlarm(bool active) { this->alarmActive = active; }

bool Context::isAlarmActive() { return this->alarmActive; }

void Context::setPreAlarm(bool active) { this->preAlarmActive = active; }

bool Context::isPreAlarmActive() { return this->preAlarmActive; }

void Context::blink() { this->ledBlinking = true; }

void Context::stopBlink() { this->ledBlinking = false; }

bool Context::isBlinking() { return this->ledBlinking; }

void Context::setLCDMessage(const char* msg) { this->lcdMessage = msg; }

const char* Context::getLCDMessage() { return this->lcdMessage; }

// ======== MESSAGE QUEUE IMPLEMENTATION ========

bool Context::addMessage(const String& msg)
{
    if (queueCount >= MSG_QUEUE_SIZE)
    {
        return false;  // Queue full
    }

    messageQueue[queueTail].content = msg;
    messageQueue[queueTail].timestamp = millis();
    messageQueue[queueTail].valid = true;

    queueTail = (queueTail + 1) % MSG_QUEUE_SIZE;
    queueCount++;
    return true;
}

bool Context::hasMessage(Pattern& pattern)
{
    unsigned long now = millis();

    // Search through entire queue, not just head
    for (int i = 0; i < queueCount; i++)
    {
        int index = (queueHead + i) % MSG_QUEUE_SIZE;

        if (messageQueue[index].valid)
        {
            // Check if message is not expired
            if ((now - messageQueue[index].timestamp) <= MSG_TIMEOUT_MS)
            {
                Msg tempMsg(messageQueue[index].content);
                if (pattern.match(tempMsg))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Context::consumeMessage(Pattern& pattern)
{
    unsigned long now = millis();

    // Search through entire queue for matching message
    for (int i = 0; i < queueCount; i++)
    {
        int index = (queueHead + i) % MSG_QUEUE_SIZE;

        if (messageQueue[index].valid)
        {
            // Check if message is not expired
            if ((now - messageQueue[index].timestamp) <= MSG_TIMEOUT_MS)
            {
                Msg tempMsg(messageQueue[index].content);
                if (pattern.match(tempMsg))
                {
                    // Mark as consumed
                    messageQueue[index].valid = false;

                    // If it was the head, advance head pointer
                    if (i == 0)
                    {
                        // Skip all invalid messages at head
                        while (queueCount > 0 && !messageQueue[queueHead].valid)
                        {
                            queueHead = (queueHead + 1) % MSG_QUEUE_SIZE;
                            queueCount--;
                        }
                    }
                    else
                    {
                        // Just decrement count, will be cleaned up later
                        queueCount--;
                    }

                    return true;
                }
            }
            else
            {
                // Message expired, mark invalid
                messageQueue[index].valid = false;
            }
        }
    }

    return false;
}

int Context::cleanExpiredMessages()
{
    unsigned long now = millis();
    int removedCount = 0;

    // Remove expired messages from head
    while (queueCount > 0 && messageQueue[queueHead].valid)
    {
        if ((now - messageQueue[queueHead].timestamp) > MSG_TIMEOUT_MS)
        {
            messageQueue[queueHead].valid = false;
            queueHead = (queueHead + 1) % MSG_QUEUE_SIZE;
            queueCount--;
            removedCount++;
        }
        else
        {
            break;  // Head is still valid, stop cleaning
        }
    }

    return removedCount;
}

bool Context::isMessageQueueFull() { return queueCount >= MSG_QUEUE_SIZE; }

int Context::getMessageQueueSize() { return queueCount; }

// ======== JSON OUTPUT INTERFACE ========

void Context::setJsonField(const String& key, const String& value) { jsonDoc[key] = value; }

void Context::setJsonField(const String& key, float value) { jsonDoc[key] = value; }

void Context::setJsonField(const String& key, int value) { jsonDoc[key] = value; }

void Context::setJsonField(const String& key, bool value) { jsonDoc[key] = value; }

void Context::removeJsonField(const String& key) { jsonDoc.remove(key); }

String Context::buildJSON()
{
    String output;
    serializeJson(jsonDoc, output);
    return output;
}

void Context::clearJsonFields() { jsonDoc.clear(); }