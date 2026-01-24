#ifndef __CONTEXT__
#define __CONTEXT__

#include <Arduino.h>
#include <ArduinoJson.h>

#include "config.hpp"
#include "kernel/CommandType.hpp"
#include "kernel/MsgService.hpp"

#define MSG_QUEUE_SIZE 10
#define LCD_BUFFER_SIZE LCD_ROW* LCD_COL + 1

struct CommandEntry
{
    const char* name;
    CommandType type;
};

class Context
{
   private:
    // DOOR FLAGS
    uint8_t openDoorRequested : 1;
    uint8_t closeDoorRequested : 1;
    uint8_t doorOpen : 1;

    // SYSTEM FLAGS
    uint8_t alarmActive : 1;
    uint8_t preAlarmActive : 1;

    // LED FLAG
    uint8_t ledBlinking : 1;

    // DRONE FLAGS
    uint8_t landingCheck : 1;
    uint8_t takeoffCheck : 1;
    uint8_t droneIn : 1;

    // SENSORS
    float currentDistance;
    float currentTemperature;
    bool pirActive;

    // LCD
    char lcdMessage[LCD_BUFFER_SIZE];

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

    // COMMAND TABLE
    static const CommandEntry commandTable[];
    static const int COMMAND_TABLE_SIZE;

    // enqueue privata
    bool enqueueCommand(CommandType cmd, uint32_t now);

   public:
    Context();

    // DOOR CONTROL
    void closeDoor();
    void openDoor();
    bool openDoorReq() const;
    bool closeDoorReq() const;
    bool isDoorOpen() const;
    void setDoorClosed();
    void setDoorOpened();

    // ALARM
    void setAlarm(bool active);
    bool isAlarmActive() const;
    void setPreAlarm(bool active);
    bool isPreAlarmActive() const;

    // LED
    void blink();
    void stopBlink();
    bool isBlinking() const;

    // LCD
    const char* getLCDMessage() const;
    void setLCDMessage(const char* msg);

    // DRONE
    void requestLandingCheck();
    void closeLandingCheck();
    bool landingCheckRequested() const;
    void requestTakeoffCheck();
    void closeTakeoffCheck();
    bool takeoffCheckRequested() const;
    void setDroneIn(bool state);
    bool isDroneIn() const;

    // COMMAND
    bool tryEnqueueMsg(const char* msg);
    bool consumeCommand(CommandType cmd);
    void cleanupExpired(uint32_t now);

    // JSON
    void setJsonField(const char* key, const char* value);
    void setJsonField(const char* key, float value);
    void setJsonField(const char* key, int value);
    void setJsonField(const char* key, bool value);
    void removeJsonField(const char* key);
    String buildJSON() const;
    void clearJsonFields();
};

#endif
