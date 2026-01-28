#include "model/Context.hpp"

#include <ArduinoJson.h>

#include "Context.hpp"
#include "config.hpp"

/**
 * @brief Command name for opening the hangar door
 */
const CommandEntry Context::commandTable[] = {{OPEN_CMD, CommandType::OPEN}};

const int Context::COMMAND_TABLE_SIZE = sizeof(Context::commandTable) / sizeof(commandTable[0]);

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
      pirActive(false),
      currentDistance(0.0f),
      commandHead(0),
      commandTail(0),
      commandCount(0),
      droneState(0)
{
    lcdMessage[0] = '\0';
}

// === DOOR CONTROL ===
void Context::closeDoor()
{
    closeDoorRequested = true;
    openDoorRequested = false;
}
void Context::openDoor()
{
    openDoorRequested = true;
    closeDoorRequested = false;
}
bool Context::openDoorReq() const { return openDoorRequested; }
bool Context::closeDoorReq() const { return closeDoorRequested; }
bool Context::isDoorOpen() const { return doorOpen; }
void Context::setDoorOpened() { doorOpen = true; }
void Context::setDoorClosed()
{
    closeDoorRequested = false;
    openDoorRequested = false;
    doorOpen = false;
}

// === ALARM & PIR ===
void Context::setAlarm(bool active) { alarmActive = active; }
bool Context::isAlarmActive() const { return alarmActive; }
void Context::setPreAlarm(bool active) { preAlarmActive = active; }
bool Context::isPreAlarmActive() const { return preAlarmActive; }
void Context::setPir(bool active) { pirActive = active; }
bool Context::isPirActive() const { return pirActive; }

// === LED ===
void Context::blink() { ledBlinking = true; }
void Context::stopBlink() { ledBlinking = false; }
bool Context::isBlinking() const { return ledBlinking; }

// === LCD ===
void Context::setLCDMessage(const char* msg)
{
    if (!msg)
    {
        lcdMessage[0] = '\0';
        return;
    }
    strncpy(lcdMessage, msg, LCD_BUFFER_SIZE - 1);
    lcdMessage[LCD_BUFFER_SIZE - 1] = '\0';
}
const char* Context::getLCDMessage() const { return lcdMessage; }

// === DRONE & SENSORS ===
void Context::setDistance(float d) { currentDistance = d; }
void Context::setDroneIn(bool state) { droneIn = state; }
bool Context::isDroneIn() const { return droneIn; }
void Context::requestLandingCheck() { landingCheck = true; }
void Context::closeLandingCheck() { landingCheck = false; }
bool Context::landingCheckRequested() const { return landingCheck; }
void Context::requestTakeoffCheck() { takeoffCheck = true; }
void Context::closeTakeoffCheck() { takeoffCheck = false; }
bool Context::takeoffCheckRequested() const { return takeoffCheck; }

void Context::setDroneState(int s) { droneState = (int8_t)s; }
int Context::getDroneState() const { return (int)droneState; }

// === COMMAND QUEUE ===
bool Context::enqueueCommand(CommandType cmd, uint16_t now)
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
            if (i != 0)
            {
                QueuedCommand temp = commandQueue[index];
                commandQueue[index] = commandQueue[commandHead];
                commandQueue[commandHead] = temp;
            }
            commandHead = (commandHead + 1) % MSG_QUEUE_SIZE;
            commandCount--;
            return true;
        }
    }
    return false;
}

void Context::cleanupExpired(uint32_t now)
{
    int removed = 0;
    for (int i = 0; i < commandCount; i++)
    {
        int index = (commandHead + i) % MSG_QUEUE_SIZE;
        if ((uint16_t)(now - commandQueue[index].timestamp) >= CONFIG_CMD_TTL_MS)
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
        return false;
    while (*msg == ' ' || *msg == '\t') msg++;
    for (int i = 0; i < COMMAND_TABLE_SIZE; i++)
    {
        if (strcasecmp(msg, commandTable[i].name) == 0)
        {
            enqueueCommand(commandTable[i].type, (uint16_t)millis());
            return true;
        }
    }
    return false;
}

void Context::serializeData(JsonDocument& doc) const
{
    const char* droneLabels[] = {DRONE_REST_STATE, DRONE_TAKING_OFF_STATE, DRONE_OPERATING_STATE,
                                 DRONE_LANDING_STATE};

    if (this->isAlarmActive())
    {
        doc[HANGAR_STATE_KEY] = HANGAR_ALARM_STATE;
    }
    else if (this->isPreAlarmActive())
    {
        doc[HANGAR_STATE_KEY] = HANGAR_PRE_ALARM_STATE;
    }
    else
    {
        doc[HANGAR_STATE_KEY] = HANGAR_NORMAL_STATE;
    }

    doc[DRONE_STATE_KEY] = droneLabels[this->droneState];

    if (this->currentDistance > 0.0f)
    {
        doc[DISTANCE_KEY] = this->currentDistance;
    }
}
