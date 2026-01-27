#ifndef __CONTEXT__
#define __CONTEXT__

#include <Arduino.h>
#include "config.hpp"
#include "kernel/CommandType.hpp"

#define MSG_QUEUE_SIZE 3
#define LCD_BUFFER_SIZE (LCD_ROW * LCD_COL + 1)

struct CommandEntry {
    const char* name;
    CommandType type;
};

class Context {
   private:
    // --- FLAG (Bit-fields) ---
    uint16_t openDoorRequested : 1;
    uint16_t closeDoorRequested : 1;
    uint16_t doorOpen : 1;
    uint16_t alarmActive : 1;
    uint16_t preAlarmActive : 1;
    uint16_t ledBlinking : 1;
    uint16_t landingCheck : 1;
    uint16_t takeoffCheck : 1;
    uint16_t droneIn : 1;
    uint16_t pirActive : 1; // Spostato nei bit-field per risparmiare 1 byte

    // --- SENSORI ---
    float currentDistance;
    float currentTemperature;

    // --- LCD BUFFER ---
    char lcdMessage[LCD_BUFFER_SIZE];

    // --- CODA COMANDI ---
    struct QueuedCommand {
        CommandType cmd;
        uint16_t timestamp;
    };
    QueuedCommand commandQueue[MSG_QUEUE_SIZE];
    int8_t commandHead;
    int8_t commandTail;
    int8_t commandCount;
    int8_t droneState;
    int8_t hangarState;

    // --- TABELLA COMANDI ---
    static const CommandEntry commandTable[];
    static const int COMMAND_TABLE_SIZE;

    bool enqueueCommand(CommandType cmd, uint16_t now);

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

    // ALARM & SYSTEM
    void setAlarm(bool active);
    bool isAlarmActive() const;
    void setPreAlarm(bool active);
    bool isPreAlarmActive() const;
    void setPir(bool active);
    bool isPirActive() const;
    void setHangarState(int s);
    int getHangarState() const;

    // LED
    void blink();
    void stopBlink();
    bool isBlinking() const;

    // LCD
    const char* getLCDMessage() const;
    void setLCDMessage(const char* msg);

    // DRONE & SENSORS
    void setDistance(float d);
    float getDistance() const;
    void setTemperature(float t);
    float getTemperature() const;
    
    void setDroneIn(bool state);
    bool isDroneIn() const;
    void requestLandingCheck();
    void closeLandingCheck();
    bool landingCheckRequested() const;
    void requestTakeoffCheck();
    void closeTakeoffCheck();
    bool takeoffCheckRequested() const;
    
    void setDroneState(int s);
    int getDroneState() const;

    // COMMAND MANAGEMENT
    bool tryEnqueueMsg(const char* msg);
    bool consumeCommand(CommandType cmd);
    void cleanupExpired(uint32_t now);

    // JSON Generation
    size_t buildJSON(Print& target) const;
};

#endif