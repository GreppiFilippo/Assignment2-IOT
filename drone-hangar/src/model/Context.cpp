#include "model/Context.hpp"

#include <string.h>

#include "Context.hpp"
#include "config.hpp"

#define JSON_DOC_SIZE 256

/**
 * @brief Command table mapping command names to CommandType enums.
 *
 */
const CommandEntry Context::commandTable[] = {{"OPEN", CommandType::OPEN}};
const int Context::COMMAND_TABLE_SIZE =
    sizeof(Context::commandTable) / sizeof(Context::commandTable[0]);

Context::Context()
    : openDoorRequested(false),
      closeDoorRequested(false),
      doorOpen(false),
      alarmActive(false),
      preAlarmActive(false),
      ledBlinking(false),
      landingCheck(false),
      takeoffCheck(false),
      droneIn(true),
      currentDistance(0),
      currentTemperature(0),
      pirActive(false),
      commandHead(0),
      commandTail(0),
      commandCount(0)
{
    lcdMessage[0] = '\0';
}

// DOOR
void Context::closeDoor() { closeDoorRequested = true; }
void Context::openDoor() { openDoorRequested = true; }
bool Context::closeDoorReq() const { return closeDoorRequested; }
bool Context::openDoorReq() const { return openDoorRequested; }
bool Context::isDoorOpen() const { return doorOpen; }
void Context::setDoorOpened() { doorOpen = true; }
void Context::setDoorClosed()
{
    closeDoorRequested = false;
    openDoorRequested = false;
    doorOpen = false;
}

// ALARM
void Context::setAlarm(bool active) { alarmActive = active; }
bool Context::isAlarmActive() const { return alarmActive; }
void Context::setPreAlarm(bool active) { preAlarmActive = active; }
bool Context::isPreAlarmActive() const { return preAlarmActive; }

// LED
void Context::blink() { ledBlinking = true; }
void Context::stopBlink() { ledBlinking = false; }
bool Context::isBlinking() const { return ledBlinking; }

// LCD
void Context::setLCDMessage(const char* msg)
{
    if (!msg)
    {
        lcdMessage[0] = '\0';
        return;
    }
    strncpy(lcdMessage, msg, sizeof(lcdMessage) - 1);
    lcdMessage[sizeof(lcdMessage) - 1] = '\0';
}
const char* Context::getLCDMessage() const { return lcdMessage; }

// DRONE
void Context::setDroneIn(bool state) { droneIn = state; }
bool Context::isDroneIn() const { return droneIn; }

void Context::requestLandingCheck() { landingCheck = true; }
void Context::closeLandingCheck() { landingCheck = false; }
bool Context::landingCheckRequested() const { return landingCheck; }

void Context::requestTakeoffCheck() { takeoffCheck = true; }
void Context::closeTakeoffCheck() { takeoffCheck = false; }
bool Context::takeoffCheckRequested() const { return takeoffCheck; }

// COMMAND QUEUE
bool Context::enqueueCommand(CommandType cmd, uint32_t now)
{
    if (commandCount >= MSG_QUEUE_SIZE)
        return false;

    commandQueue[commandTail] = {cmd, now};
    commandTail = (commandTail + 1) % MSG_QUEUE_SIZE;
    commandCount++;
    return true;
}

bool Context::consumeCommand(CommandType cmd)
{
    int scanned = 0;
    while (scanned < commandCount)
    {
        int index = (commandHead + scanned) % MSG_QUEUE_SIZE;
        if (commandQueue[index].cmd == cmd)
        {
            if (scanned != 0)
            {
                QueuedCommand temp = commandQueue[index];
                commandQueue[index] = commandQueue[commandHead];
                commandQueue[commandHead] = temp;
            }

            commandHead = (commandHead + 1) % MSG_QUEUE_SIZE;
            commandCount--;
            return true;
        }
        scanned++;
    }
    return false;
}

void Context::cleanupExpired(uint32_t now)
{
    int removed = 0;
    for (int i = 0; i < commandCount; i++)
    {
        int index = (commandHead + i) % MSG_QUEUE_SIZE;
        if ((now - commandQueue[index].timestamp) >= CONFIG_CMD_TTL_MS)
        {
            removed++;
        }
        else if (removed > 0)
        {
            commandQueue[(commandHead + i - removed) % MSG_QUEUE_SIZE] = commandQueue[index];
        }
    }
    commandHead = (commandHead + removed) % MSG_QUEUE_SIZE;
    commandCount -= removed;
}

bool Context::tryEnqueueMsg(const char* msg)
{
    if (!msg)
    {
        return false;
    }

    while (*msg == ' ' || *msg == '\t')
    {
        msg++;
    }

    for (int i = 0; i < COMMAND_TABLE_SIZE; i++)
    {
        if (strcasecmp(msg, commandTable[i].name) == 0)
        {
            enqueueCommand(commandTable[i].type, millis());
            return true;
        }
    }
    return false;
}

void Context::setJsonField(const char* key, const char* value) { jsonDoc[key] = value; }
void Context::setJsonField(const char* key, float value) { jsonDoc[key] = value; }
void Context::setJsonField(const char* key, int value) { jsonDoc[key] = value; }
void Context::setJsonField(const char* key, bool value) { jsonDoc[key] = value; }
void Context::removeJsonField(const char* key) { jsonDoc.remove(key); }

String Context::buildJSON() const
{
    char buffer[JSON_DOC_SIZE];
    serializeJson(this->jsonDoc, buffer);
    return String(buffer);
}

void Context::clearJsonFields() { jsonDoc.clear(); }