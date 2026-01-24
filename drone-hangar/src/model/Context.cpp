#include "model/Context.hpp"

#include <string.h>  // per strcasecmp

#include "config.hpp"

// COMMAND TABLE
const CommandEntry Context::commandTable[] = {{"OPEN", CommandType::OPEN}};
const int Context::COMMAND_TABLE_SIZE =
    sizeof(Context::commandTable) / sizeof(Context::commandTable[0]);

// CONSTRUCTOR
Context::Context()
    : openDoorRequested(false),
      closeDoorRequested(false),
      doorOpen(false),
      alarmActive(false),
      preAlarmActive(false),
      currentDistance(0),
      currentTemperature(0),
      pirActive(false),
      lcdMessage(nullptr),
      ledBlinking(false),
      landingCheck(false),
      takeoffCheck(false),
      droneIn(true),
      commandHead(0),
      commandTail(0),
      commandCount(0)
{
}

// DOOR
void Context::closeDoor() { closeDoorRequested = true; }
void Context::openDoor() { openDoorRequested = true; }
bool Context::closeDoorReq() { return closeDoorRequested; }
bool Context::openDoorReq() { return openDoorRequested; }
bool Context::isDoorOpen() { return doorOpen; }
void Context::setDoorOpened() { doorOpen = true; }
void Context::setDoorClosed()
{
    closeDoorRequested = false;
    openDoorRequested = false;
    doorOpen = false;
}

// ALARM
void Context::setAlarm(bool active) { alarmActive = active; }
bool Context::isAlarmActive() { return alarmActive; }
void Context::setPreAlarm(bool active) { preAlarmActive = active; }
bool Context::isPreAlarmActive() { return preAlarmActive; }

// LED
void Context::blink() { ledBlinking = true; }
void Context::stopBlink() { ledBlinking = false; }
bool Context::isBlinking() { return ledBlinking; }

// LCD
void Context::setLCDMessage(const char* msg) { lcdMessage = msg; }
const char* Context::getLCDMessage() { return lcdMessage; }

// DRONE
void Context::setDroneIn(bool state) { droneIn = state; }
bool Context::isDroneIn() { return droneIn; }

void Context::requestLandingCheck() { landingCheck = true; }
void Context::closeLandingCheck() { landingCheck = false; }
bool Context::landingCheckRequested() { return landingCheck; }

void Context::requestTakeoffCheck() { takeoffCheck = true; }
void Context::closeTakeoffCheck() { takeoffCheck = false; }
bool Context::takeoffCheckRequested() { return takeoffCheck; }

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
    for (int i = 0; i < commandCount; i++)
    {
        int index = (commandHead + i) % MSG_QUEUE_SIZE;
        if (commandQueue[index].cmd == cmd)
        {
            for (int j = i; j < commandCount - 1; j++)
            {
                int from = (commandHead + j + 1) % MSG_QUEUE_SIZE;
                int to = (commandHead + j) % MSG_QUEUE_SIZE;
                commandQueue[to] = commandQueue[from];
            }
            commandTail = (commandTail - 1 + MSG_QUEUE_SIZE) % MSG_QUEUE_SIZE;
            commandCount--;
            return true;
        }
    }
    return false;
}

void Context::cleanupExpired(uint32_t now)
{
    for (int i = 0; i < commandCount;)
    {
        int index = (commandHead + i) % MSG_QUEUE_SIZE;
        if ((now - commandQueue[index].timestamp) >= CONFIG_CMD_TTL_MS)
        {
            for (int j = i; j < commandCount - 1; j++)
            {
                int from = (commandHead + j + 1) % MSG_QUEUE_SIZE;
                int to = (commandHead + j) % MSG_QUEUE_SIZE;
                commandQueue[to] = commandQueue[from];
            }
            commandTail = (commandTail - 1 + MSG_QUEUE_SIZE) % MSG_QUEUE_SIZE;
            commandCount--;
        }
        else
            i++;
    }
}

// tryEnqueueMsg ottimizzato per const char* senza String temporanei
bool Context::tryEnqueueMsg(const char* msg)
{
    if (!msg)
        return false;

    // rimuovi spazi iniziali/finali
    while (*msg == ' ' || *msg == '\t') msg++;

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

// JSON
void Context::setJsonField(const char* key, const char* value) { jsonDoc[key] = value; }
void Context::setJsonField(const char* key, float value) { jsonDoc[key] = value; }
void Context::setJsonField(const char* key, int value) { jsonDoc[key] = value; }
void Context::setJsonField(const char* key, bool value) { jsonDoc[key] = value; }
void Context::removeJsonField(const char* key) { jsonDoc.remove(key); }

String Context::buildJSON()
{
    String out;
    serializeJson(jsonDoc, out);
    return out;
}

void Context::clearJsonFields() { jsonDoc.clear(); }