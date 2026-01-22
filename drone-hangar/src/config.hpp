#ifndef __CONFIG__
#define __CONFIG__

/* ===== Pin definitions ===== */
#define TEMP_PIN A0  // Temperature sensor pin TMP36 - analog input
#define L1_PIN 2     // L1 green led pin - indicate on
#define L2_PIN 5     // L2 green led pin (must be pwm~) - indicate in action
#define L3_PIN 4     // L3 red led pin - indicate alarm

#define RESET_PIN 8   // RESET button pin - tactile button
#define DPD_PIN 9     // Drone presence detector pin - PIR sensor
#define HD_PIN 11     // Drone Hangar door pin - servo motor
#define DDD_PIN_E 12  // Drone Distance Detector pin - ultrasonic sensor
#define DDD_PIN_T 13
#define TEMP_PIN A0

#define LCD_ADR 0x27
#define LCD_COL 16
#define LCD_ROW 2

// TODO: Adjust pin definitions as per your hardware setup
/* ===== Distance thresholds (cm) ===== */
#define D1 100  // Distance threshold for drone exit detection
#define D2 10   // Distance threshold for drone landing detection

/* ===== Time thresholds (milliseconds) ===== */
#define TIME1 3000  // Time to confirm drone has exited (3 seconds)
#define TIME2 3000  // Time to confirm drone has landed (3 seconds)
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

/* ===== LCD message definitions ===== */
#define LCD_REST_STATE "DRONE INSIDE"    // Drone is inside hangar and at rest
#define LCD_TAKING_OFF_STATE "TAKE OFF"  // Drone is taking off
#define LCD_OPERATING_STATE "DRONE OUT"  // Drone is operating outside
#define LCD_LANDING_STATE "LANDING"      // Drone is landing
#define LCD_ALARM_STATE "ALARM"          // Hangar in normal state

/* ===== Drone state definitions serial ===== */
#define DRONE_REST_STATE "rest"              // Drone is inside hangar and at rest
#define DRONE_TAKING_OFF_STATE "taking_off"  // Drone is taking off
#define DRONE_OPERATING_STATE "operating"    // Drone is operating outside
#define DRONE_LANDING_STATE "landing"        // Drone is landing

/* ===== Hangar state definitions ===== */
#define HANGAR_NORMAL_STATE "normal"  // Hangar in normal state
#define HANGAR_ALARM_STATE "alarm"    // Hangar in alarm state

/* ===== Command definitions ===== */
#define OPEN_CMD "OPEN DOOR"  // Command to open the hangar door

#endif
