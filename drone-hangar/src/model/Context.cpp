#include "model/Context.hpp"

#include <string.h>

#include "Context.hpp"
#include "config.hpp"

// dtostrf is provided by avr-libc for float->string formatting on Arduino
extern "C" char* dtostrf(double __val, signed char __width, unsigned char __prec, char* __s);

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

void setDistance(float distance);
void Context::setDistance(float distance) { this->currentDistance = distance; }

void setDroneState(DroneState state);
void Context::setDroneState(DroneState state) { this->droneState = state; }
void Context::setHangarState(HangarState state) { this->hangarState = state; }

void Context::serializeData(JsonDocument& doc) const
{
    doc[JSON_HANGAR_STATE] = hangarStateToJson();
    doc[JSON_DRONE_STATE] = droneStateToJson();

    if (this->currentDistance >= 0)
    {
        char distBuf[16];
        dtostrf(this->currentDistance, 0, 2, distBuf);  // two decimal places
        doc[JSON_DISTANCE] = distBuf;
    }
}

const char* Context::hangarStateToJson() const
{
    switch (this->hangarState)
    {
        case ALARM:
            return HANGAR_ALARM_STATE;

        case NORMAL:
        case TRACKING_PRE_ALARM:
        case PREALARM:
        case TRACKING_ALARM:
        default:
            return HANGAR_NORMAL_STATE;
    }
}

const char* Context::droneStateToJson() const
{
    switch (this->droneState)
    {
        case REST:
            return DRONE_REST_STATE;

        case TAKING_OFF:
            return DRONE_TAKING_OFF_STATE;

        case OPERATING:
            return DRONE_OPERATING_STATE;

        case LANDING:
            return DRONE_LANDING_STATE;
        default:
            return DRONE_REST_STATE;
    }
}