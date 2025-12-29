#ifndef __CONTEXT__
#define __CONTEXT__

// TODO:
class Context {
    private:
        // System state
        SystemState currentSystemState;
        DoorState currentDoorState;
        
        // Door control requests (auto-clear)
        bool openDoorRequested;
        bool closeDoorRequested;
        
        // System flags
        bool alarmActive;
        bool preAlarmActive;
        
        // Sensor data
        float currentDistance;
        float currentTemperature;
        bool pirActive;
        
        // Command requests (auto-clear)
        bool takeoffRequested;
        bool landingRequested;

    public:
        Context();
        
        // === DOOR CONTROL ===
        /**
         * @brief Request to open the door.
         * 
         */
        void requestOpenDoor();
        bool isOpenDoorRequested();
        void requestCloseDoor();
        bool isCloseDoorRequested(); 
        bool isDoorOpen();
        void setDoorClosed();
        void setDoorOpened();

        
        // === SYSTEM STATE ===
        void setSystemState(SystemState state);
        SystemState getSystemState();
        
        // === TEMP ALARM TASK ===
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
        
        // === COMMANDS ===
        void requestTakeoff();
        bool isTakeoffRequested();
        void requestLanding();
        bool isLandingRequested();
        
        // === SENSOR DATA ===
        void updateDistance(float dist);
        float getDistance();
        void updateTemperature(float temp);
        float getTemperature();
        void setPirActive(bool active);
        bool isPirActive();
        
        // === UTILITY ===
        bool canExecuteTakeoff();
        bool canExecuteLanding();
};

#endif
