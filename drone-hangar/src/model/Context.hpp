#ifndef __CONTEXT__
#define __CONTEXT__

#include <Arduino.h>
#include <ArduinoJson.h>

#include "kernel/CommandType.hpp"
#include "kernel/MsgService.hpp"

#define MSG_QUEUE_SIZE 10

struct CommandEntry
{
    const char* name;
    CommandType type;
};

/**
 * @brief Class representing the shared context of the system.
 */
class Context
{
   private:
    // ===== DOOR CONTROL =====
    bool openDoorRequested;
    bool closeDoorRequested;
    bool doorOpen;

    // ===== SYSTEM FLAGS =====
    bool alarmActive;
    bool preAlarmActive;

    // ===== SENSOR DATA =====
    float currentDistance;
    float currentTemperature;
    bool pirActive;

    // ===== LCD =====
    const char* lcdMessage;

    // ===== LED =====
    bool ledBlinking;

    // ===== DRONE =====
    bool landingCheck;
    bool takeoffCheck;
    bool droneIn;

    // ===== COMMAND QUEUE =====
    struct QueuedCommand
    {
        CommandType cmd;
        uint32_t timestamp;
    };
    QueuedCommand commandQueue[MSG_QUEUE_SIZE];
    int commandHead;
    int commandTail;
    int commandCount;

    // ===== JSON OUTPUT FIELDS =====
    JsonDocument jsonDoc;
    bool enqueueCommand(CommandType cmd, uint32_t now);

    // ===== COMMAND TABLE =====
    static const CommandEntry commandTable[];
    static const int COMMAND_TABLE_SIZE;

   public:
    Context();

    // ===== DOOR CONTROL =====
    void closeDoor();
    void openDoor();
    bool openDoorReq();
    bool closeDoorReq();
    bool isDoorOpen();
    void setDoorClosed();
    void setDoorOpened();

    // ===== ALARM =====
    void setAlarm(bool active);
    bool isAlarmActive();
    void setPreAlarm(bool active);
    bool isPreAlarmActive();

    // ===== BLINKING =====
    void blink();
    void stopBlink();
    bool isBlinking();

    // ===== LCD =====
    const char* getLCDMessage();
    void setLCDMessage(const char* msg);

    // ===== DRONE STATE =====
    void requestLandingCheck();
    void closeLandingCheck();
    bool landingCheckRequested();
    void requestTakeoffCheck();
    void closeTakeoffCheck();
    bool takeoffCheckRequested();
    void setDroneIn(bool state);
    bool isDroneIn();

    // ===== COMMAND INTERFACE =====
    /**
     * @brief Try to enqueue a command based on message content.
     * Messages not in the table are ignored automatically.
     *
     * @param msg Message to process
     * @return true if command recognized and enqueued
     */
    bool tryEnqueueMsg(const String& msg);

    bool consumeCommand(CommandType cmd);

    // Remove expired commands based on timestamp
    void cleanupExpired(uint32_t now);

    // ===== JSON OUTPUT INTERFACE =====
    void setJsonField(const String& key, const String& value);
    void setJsonField(const String& key, float value);
    void setJsonField(const String& key, int value);
    void setJsonField(const String& key, bool value);
    void removeJsonField(const String& key);
    String buildJSON();
    void clearJsonFields();
};

#endif