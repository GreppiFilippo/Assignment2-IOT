#ifndef __CONTEXT__
#define __CONTEXT__

// TODO:

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

   public:
    Context();

    // === DOOR CONTROL ===

    /**
     * @brief Request to open the door.
     *
     */
    void closeDoor();
    void openDoor();
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
};

#endif
