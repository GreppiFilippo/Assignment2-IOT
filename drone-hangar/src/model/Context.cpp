#include "model/Context.hpp"

#include "config.hpp"

// ===== COMMAND TABLE =====
const CommandEntry Context::commandTable[] = {{"OPEN", CommandType::OPEN}};
const int Context::COMMAND_TABLE_SIZE =
    sizeof(Context::commandTable) / sizeof(Context::commandTable[0]);

// ===== CONSTRUCTOR =====
Context::Context()
{
    openDoorRequested = false;
    closeDoorRequested = false;
    doorOpen = false;
    alarmActive = false;
    preAlarmActive = false;

    currentDistance = 0;
    currentTemperature = 0;
    pirActive = false;

    lcdMessage = "";
    ledBlinking = false;
    droneIn = true;

    landingCheck = false;
    takeoffCheck = false;

    // Command queue init
    commandHead = 0;
    commandTail = 0;
    commandCount = 0;
}

// ===== DOOR CONTROL =====
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

// ===== ALARM =====
void Context::setAlarm(bool active) { alarmActive = active; }
bool Context::isAlarmActive() { return alarmActive; }
void Context::setPreAlarm(bool active) { preAlarmActive = active; }
bool Context::isPreAlarmActive() { return preAlarmActive; }

// ===== BLINKING =====
void Context::blink() { ledBlinking = true; }
void Context::stopBlink() { ledBlinking = false; }
bool Context::isBlinking() { return ledBlinking; }

// ===== LCD =====
void Context::setLCDMessage(const char* msg) { lcdMessage = msg; }
const char* Context::getLCDMessage() { return lcdMessage; }

// ===== DRONE =====
void Context::setDroneIn(bool state) { droneIn = state; }
bool Context::isDroneIn() { return droneIn; }

void Context::requestLandingCheck() { landingCheck = true; }
void Context::closeLandingCheck() { landingCheck = false; }
bool Context::landingCheckRequested() { return landingCheck; }

void Context::requestTakeoffCheck() { takeoffCheck = true; }
void Context::closeTakeoffCheck() { takeoffCheck = false; }
bool Context::takeoffCheckRequested() { return takeoffCheck; }

// ===== COMMAND QUEUE =====
bool Context::enqueueCommand(CommandType cmd, uint32_t now)
{
    if (commandCount >= MSG_QUEUE_SIZE)
        return false;

    commandQueue[commandTail].cmd = cmd;
    commandQueue[commandTail].timestamp = now;
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
            // shift left
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

// ===== TRY ENQUEUE MESSAGE =====

bool Context::tryEnqueueMsg(const String& msg)
{
    // Normalize incoming message: trim whitespace and compare case-insensitively
    String s = msg;
    s.trim();
    s.toUpperCase();

    for (int i = 0; i < COMMAND_TABLE_SIZE; i++)
    {
        String name = String(commandTable[i].name);

        if (s.equals(name))
        {
            enqueueCommand(commandTable[i].type, millis());
            return true;  // command recognized and enqueued
        }
    }

    return false;  // unknown command ignored
}

void Context::cleanupExpired(uint32_t now)
{
    for (int i = 0; i < commandCount;)
    {
        int index = (commandHead + i) % MSG_QUEUE_SIZE;
        uint32_t ts = commandQueue[index].timestamp;
        if ((now - ts) >= CONFIG_CMD_TTL_MS)
        {
            // remove this element by shifting left
            for (int j = i; j < commandCount - 1; j++)
            {
                int from = (commandHead + j + 1) % MSG_QUEUE_SIZE;
                int to = (commandHead + j) % MSG_QUEUE_SIZE;
                commandQueue[to] = commandQueue[from];
            }
            commandTail = (commandTail - 1 + MSG_QUEUE_SIZE) % MSG_QUEUE_SIZE;
            commandCount--;
            // do not increment i, check the new element at this position
        }
        else
        {
            i++;
        }
    }
}

// ===== JSON OUTPUT =====
void Context::setJsonField(const String& key, const String& value) { jsonDoc[key] = value; }
void Context::setJsonField(const String& key, float value) { jsonDoc[key] = value; }
void Context::setJsonField(const String& key, int value) { jsonDoc[key] = value; }
void Context::setJsonField(const String& key, bool value) { jsonDoc[key] = value; }
void Context::removeJsonField(const String& key) { jsonDoc.remove(key); }

String Context::buildJSON()
{
    String out;
    serializeJson(jsonDoc, out);
    return out;
}

void Context::clearJsonFields() { jsonDoc.clear(); }