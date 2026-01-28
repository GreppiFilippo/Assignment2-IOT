#ifndef __CONTEXT__
#define __CONTEXT__

#include <Arduino.h>
#include <ArduinoJson.h>

#include "config.hpp"
#include "kernel/CommandType.hpp"

/** * @brief Max number of commands stored in the circular buffer.
 */
#define MSG_QUEUE_SIZE 3

/** * @brief Size of the LCD buffer including the null terminator.
 */
#define LCD_BUFFER_SIZE 32

/**
 * @struct CommandEntry
 * @brief Maps a string command name to its corresponding CommandType enum.
 */
struct CommandEntry
{
    const char* name;
    CommandType type;
};

/**
 * @class Context
 * @brief State Machine Context that centralizes sensor data and control logic.
 * * Uses bit-fields to minimize RAM footprint and a circular buffer for command queuing.
 */
class Context
{
   private:
    // --- FLAGS (Bit-fields: 10 bits total, occupies 2 bytes instead of 10) ---
    uint16_t openDoorRequested : 1;  /**< Pending request to open the door */
    uint16_t closeDoorRequested : 1; /**< Pending request to close the door */
    uint16_t doorOpen : 1;           /**< Physical state: true if door is open */
    uint16_t alarmActive : 1;        /**< Critical alarm state */
    uint16_t preAlarmActive : 1;     /**< Warning/Pre-alarm state */
    uint16_t ledBlinking : 1;        /**< Toggle for LED blinking task */
    uint16_t landingCheck : 1;       /**< Request for landing validation */
    uint16_t takeoffCheck : 1;       /**< Request for takeoff validation */
    uint16_t droneIn : 1;            /**< Presence of the drone inside the hangar */
    uint16_t pirActive : 1;          /**< PIR sensor detection state */

    // --- SENSORS ---
    float currentDistance;    /**< Distance detected by sonar sensor */
    float currentTemperature; /**< Temperature detected by environmental sensor */

    // --- LCD BUFFER ---
    char lcdMessage[LCD_BUFFER_SIZE]; /**< Buffer for the text displayed on the LCD */

    // --- COMMAND QUEUE ---
    /**
     * @struct QueuedCommand
     * @brief Internal structure for commands with TTL (Time To Live) management.
     */
    struct QueuedCommand
    {
        CommandType cmd;
        uint16_t timestamp;
    };
    QueuedCommand commandQueue[MSG_QUEUE_SIZE];
    int8_t commandHead;
    int8_t commandTail;
    int8_t commandCount;
    int8_t droneState;
    int8_t hangarState;

    // --- COMMAND TABLE ---
    static const CommandEntry commandTable[];
    static const int COMMAND_TABLE_SIZE;

    /**
     * @brief Pushes a command into the circular buffer.
     * @param cmd Command type.
     * @param now Current timestamp in milliseconds.
     * @return true if successful, false if queue is full.
     */
    bool enqueueCommand(CommandType cmd, uint16_t now);

   public:
    Context();

    /** @name Door Control */
    ///@{
    void closeDoor();
    void openDoor();
    bool openDoorReq() const;
    bool closeDoorReq() const;
    bool isDoorOpen() const;
    void setDoorClosed();
    void setDoorOpened();
    ///@}

    /** @name Alarm & System Logic */
    ///@{
    void setAlarm(bool active);
    bool isAlarmActive() const;
    void setPreAlarm(bool active);
    bool isPreAlarmActive() const;
    void setPir(bool active);
    bool isPirActive() const;
    void setHangarState(int s);
    int getHangarState() const;
    ///@}

    /** @name Visual Feedback */
    ///@{
    void blink();
    void stopBlink();
    bool isBlinking() const;
    const char* getLCDMessage() const;
    void setLCDMessage(const char* msg);
    ///@}

    /** @name Drone & Sensor Management */
    ///@{
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
    ///@}

    /** @name Command Handling */
    ///@{
    /**
     * @brief Parses a string message and enqueues a command if valid.
     * @param msg Input string (e.g., from Serial or Network).
     * @return true if the command was recognized and queued.
     */
    bool tryEnqueueMsg(const char* msg);

    /**
     * @brief Searches and consumes a specific command from the queue.
     * @param cmd The command type to look for.
     * @return true if found and removed.
     */
    bool consumeCommand(CommandType cmd);

    /**
     * @brief Removes commands from the queue that have exceeded CONFIG_CMD_TTL_MS.
     * @param now Current uptime in milliseconds (millis()).
     */
    void cleanupExpired(uint32_t now);
    ///@}

    /**
     * @brief Serializes the current state into a JSON document.
     *
     * Adds keys here to keep the API consistent. Remember to update the JSON_IN_SIZE in config.hpp
     * if needed.
     *
     * @param doc Reference to the ArduinoJson document.
     */
    void serializeData(JsonDocument& doc) const;
};

#endif