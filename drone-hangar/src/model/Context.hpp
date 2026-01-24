#ifndef __CONTEXT__
#define __CONTEXT__

#include <Arduino.h>
#include <ArduinoJson.h>

#include "kernel/CommandType.hpp"
#include "kernel/MsgService.hpp"

#define MSG_QUEUE_SIZE 10
#define JSON_DOC_SIZE 256

struct CommandEntry
{
    const char* name;
    CommandType type;
};

class Context
{
   private:
    // DOOR
    bool openDoorRequested;
    bool closeDoorRequested;
    bool doorOpen;

    // SYSTEM FLAGS
    bool alarmActive;
    bool preAlarmActive;

    // SENSORS
    float currentDistance;
    float currentTemperature;
    bool pirActive;

    // LCD
    const char* lcdMessage;

    // LED
    bool ledBlinking;

    // DRONE
    bool landingCheck;
    bool takeoffCheck;
    bool droneIn;

    // COMMAND QUEUE
    struct QueuedCommand
    {
        CommandType cmd;
        uint32_t timestamp;
    };
    QueuedCommand commandQueue[MSG_QUEUE_SIZE];
    int commandHead;
    int commandTail;
    int commandCount;

    // JSON
    JsonDocument jsonDoc;

    bool enqueueCommand(CommandType cmd, uint32_t now);

    // COMMAND TABLE
    static const CommandEntry commandTable[];
    static const int COMMAND_TABLE_SIZE;

   public:
    Context();

    // DOOR CONTROL
    void closeDoor();
    void openDoor();
    bool openDoorReq();
    bool closeDoorReq();
    bool isDoorOpen();
    void setDoorClosed();
    void setDoorOpened();

    // ALARM
    void setAlarm(bool active);
    bool isAlarmActive();
    void setPreAlarm(bool active);
    bool isPreAlarmActive();

    // LED
    void blink();
    void stopBlink();
    bool isBlinking();

    // LCD
    const char* getLCDMessage();
    void setLCDMessage(const char* msg);

    // DRONE
    void requestLandingCheck();
    void closeLandingCheck();
    bool landingCheckRequested();
    void requestTakeoffCheck();
    void closeTakeoffCheck();
    bool takeoffCheckRequested();
    void setDroneIn(bool state);
    bool isDroneIn();

    // COMMAND
    bool tryEnqueueMsg(const char* msg);  // usa const char* per risparmiare RAM
    bool consumeCommand(CommandType cmd);
    void cleanupExpired(uint32_t now);

    // JSON
    void setJsonField(const char* key, const char* value);
    void setJsonField(const char* key, float value);
    void setJsonField(const char* key, int value);
    void setJsonField(const char* key, bool value);
    void removeJsonField(const char* key);
    String buildJSON();
    void clearJsonFields();
};

#endif