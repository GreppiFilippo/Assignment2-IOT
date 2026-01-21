#ifndef __CONTEXT__
#define __CONTEXT__

#include <Arduino.h>
#include <ArduinoJson.h>
#include "kernel/MsgService.hpp"

#define MSG_QUEUE_SIZE 10
#define MSG_TIMEOUT_MS 5000  // Messages older than 5 seconds are discarded

/**
 * @brief Structure for timestamped messages in the queue.
 */
struct TimestampedMsg
{
    String content;
    unsigned long timestamp;
    bool valid;  // false if slot is empty
};

/**
 * @brief Class representing the shared context of the system.
 *
 */
class Context
{
   private:
    // Door control requests (auto-clear)
    bool openDoorRequested;
    bool closeDoorRequested;
    bool doorOpen;

    // System flags
    bool alarmActive;
    bool preAlarmActive;

    // Sensor data
    float currentDistance;
    float currentTemperature;
    bool pirActive;

    // LCD message to display
    const char* lcdMessage;

    //blinking led
    bool ledBlinking;

    // Message queue (FIFO with timestamp)
    TimestampedMsg messageQueue[MSG_QUEUE_SIZE];
    int queueHead;
    int queueTail;
    int queueCount;

    // JSON document for outgoing messages
    JsonDocument jsonDoc;

   public:
    Context();

    // === DOOR CONTROL ===

    /**
     * @brief Request to open the door.
     *
     */
    void closeDoor();
    void openDoor();
    bool openDoorReq();
    bool closeDoorReq();
    bool isDoorOpen();
    void setDoorClosed();
    void setDoorOpened();

    // ======== TEMP ALARM TASK ========
    /**
     * @brief Set the Alarm state
     *
     * @param active true to activate the alarm, false to deactivate
     */
    void setAlarm(bool active);

    /**
     * @brief Check if the alarm is active.
     *
     * @return true if the alarm is active
     * @return false if the alarm is not active
     */
    bool isAlarmActive();

    /**
     * @brief Set the Pre Alarm state
     *
     * @param active true to activate the pre-alarm, false to deactivate
     */
    void setPreAlarm(bool active);

    /**
     * @brief Check if the pre-alarm is active.
     *
     * @return true if the pre-alarm is active
     * @return false if the pre-alarm is not active
     */
    bool isPreAlarmActive();

    // ======== BLINKING TASK ========

    void blink();
    void stopBlink();
    bool isBlinking();

    // ======== LCD data ========
    const char* getLCDMessage();
    void setLCDMessage(const char* msg);

    // ======== MESSAGE QUEUE ========

    /**
     * @brief Add a message to the queue with current timestamp.
     * 
     * @param msg Message to add
     * @return true if added successfully, false if queue full
     */
    bool addMessage(const String& msg);

    /**
     * @brief Check if a message matching the pattern exists in the queue.
     * Only considers messages within MSG_TIMEOUT_MS age.
     * 
     * @param pattern Pattern to match
     * @return true if matching message found
     */
    bool hasMessage(Pattern& pattern);

    /**
     * @brief Consume (remove) the FIRST (oldest) message in queue if it matches pattern.
     * FIFO behavior: only checks head of queue.
     * 
     * @param pattern Pattern to match
     * @return true if message was consumed, false if no match
     */
    bool consumeMessage(Pattern& pattern);

    /**
     * @brief Remove expired messages from the queue (older than MSG_TIMEOUT_MS).
     * Should be called periodically by MsgTask.
     * 
     * @return Number of messages removed
     */
    int cleanExpiredMessages();

    /**
     * @brief Check if message queue is full.
     */
    bool isMessageQueueFull();

    /**
     * @brief Get number of messages in queue.
     */
    int getMessageQueueSize();

    // ======== JSON OUTPUT INTERFACE ========

    /**
     * @brief Set a JSON field with string value.
     * 
     * @param key Field name
     * @param value Field value
     */
    void setJsonField(const String& key, const String& value);

    /**
     * @brief Set a JSON field with float value.
     * 
     * @param key Field name
     * @param value Field value
     */
    void setJsonField(const String& key, float value);

    /**
     * @brief Set a JSON field with int value.
     * 
     * @param key Field name
     * @param value Field value
     */
    void setJsonField(const String& key, int value);

    /**
     * @brief Set a JSON field with boolean value.
     * 
     * @param key Field name
     * @param value Field value
     */
    void setJsonField(const String& key, bool value);

    /**
     * @brief Remove a JSON field from output.
     * 
     * @param key Field name to remove
     */
    void removeJsonField(const String& key);

    /**
     * @brief Build JSON string from all active fields.
     * 
     * @return String containing JSON object
     */
    String buildJSON();

    /**
     * @brief Clear all JSON fields.
     */
    void clearJsonFields();
};

#endif
