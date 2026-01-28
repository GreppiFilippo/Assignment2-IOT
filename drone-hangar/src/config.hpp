#ifndef __CONFIG__
#define __CONFIG__

/* ===== Pin definitions ===== */
#define TEMP_PIN A0  // Temperature sensor pin TMP36 - analog input
#define L1_PIN 2     // L1 green led pin - indicate on
#define L2_PIN 4     // L2 green led pin - indicate in action
#define L3_PIN 6     // L3 red led pin - indicate alarm

#define RESET_PIN 8   // RESET button pin - tactile button
#define DPD_PIN 9     // Drone presence detector pin - PIR sensor
#define HD_PIN 11     // Drone Hangar door pin - servo motor
#define DDD_PIN_E 12  // Drone Distance Detector echo pin - ultrasonic sensor
#define DDD_PIN_T 13  // Drone Distance Detector trigger pin - ultrasonic sensor
#define TEMP_PIN A0   // Temperature sensor pin TMP36 - analog input

#define LCD_ADR 0x27
#define LCD_COL 20
#define LCD_ROW 4

/* ====== DOOR CONFIG ====== */
#define DOOR_OPEN_ANGLE 180
#define MOVING_TIME 500

/* ===== Distance thresholds (cm) ===== */
#define D1 2    // Distance threshold for drone exit detection
#define D2 0.6  // Distance threshold for drone landing detection

/* ===== Time thresholds (milliseconds) ===== */
#define TIME1 5000  // Time to confirm drone has exited (5 seconds)
#define TIME2 5000  // Time to confirm drone has landed (5 seconds)
#define TIME3 5000  // Time to enter pre-alarm state (5 seconds)
#define TIME4 5000  // Time to enter alarm state (5 seconds)

/* ===== LED blink periods ===== */
// LED blink period (milliseconds)
#define L2_BLINK_PERIOD 500  // L2 blinks with period 0.5 second during take-off/landing

// Temperature thresholds (Celsius)
#define TEMP1 30  // Pre-alarm temperature threshold
#define TEMP2 40  // Alarm temperature threshold

/* ===== Message update period ===== */
#define JSON_UPDATE_PERIOD_MS 500  // Send JSON state update every 500ms

/* ===== Command TTL (milliseconds) ===== */
#define CONFIG_CMD_TTL_MS 5000  // Commands older than this are dropped from queue
#define BAUD_RATE 115200        // Serial communication baud rate

/* ===== LCD message definitions ===== */
#define LCD_REST_STATE "DRONE INSIDE"    // Drone is inside hangar and at rest
#define LCD_TAKING_OFF_STATE "TAKE OFF"  // Drone is taking off
#define LCD_OPERATING_STATE "DRONE OUT"  // Drone is operating outside
#define LCD_LANDING_STATE "LANDING"      // Drone is landing
#define LCD_ALARM_STATE "ALARM"          // Hangar in normal state

// ---------------- API FOR SERIAL ---------------- //
// JSON document sizes, to calculate memory to allocate follow assitant on
// https://arduinojson.org/v6/assistant

#define JSON_OUT_SIZE 128  // Size of the JSON document for output messages
#define JSON_IN_SIZE 96    // Size of the JSON document for input messages

/* ===== Drone state definitions serial ===== */
#define DRONE_STATE_KEY "drone"  // Key for drone state in messages
// Drone state values
#define DRONE_REST_STATE "rest"              // Drone is inside hangar and at rest
#define DRONE_TAKING_OFF_STATE "taking_off"  // Drone is taking off
#define DRONE_OPERATING_STATE "operating"    // Drone is operating outside
#define DRONE_LANDING_STATE "landing"        // Drone is landing

/* ===== Hangar state definitions ===== */
#define HANGAR_STATE_KEY "hangar"  // Key for hangar state in messages
// Hangar state values
#define HANGAR_NORMAL_STATE "normal"  // Hangar in normal state
#define HANGAR_ALARM_STATE "alarm"    // Hangar in alarm state

/* ===== Command definitions ===== */
#define COMMAND "cmd"  // Command key in messages
// Command values
#define OPEN_CMD "open"  // Command value to open the hangar door

/* ===== Distance definitions ===== */
#define DISTANCE_KEY "distance"  // Key for distance value in messages

/* ===== Other definitions ===== */
#define ALIVE "alive"  // Key indicating system is alive

#endif