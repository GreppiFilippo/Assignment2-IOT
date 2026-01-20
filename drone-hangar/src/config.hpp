#ifndef __CONFIG__
#define __CONFIG__

/* ===== Pin definitions ===== */
#define TEMP_PIN A0  // Temperature sensor pin TMP36 - analog input
#define L1_PIN 2     // L1 green led pin - indicate on
#define L2_PIN 5     // L2 green led pin (must be pwm~) - indicate in action
#define L3_PIN 4     // L3 red led pin - indicate alarm

#define RESET_PIN 8  // RESET button pin - tactile button
#define DPD_PIN 9    // Drone presence detector pin - PIR sensor
#define HD_PIN 11    // Drone Hangar door pin - servo motor
#define DDD_PIN 7    // Drone Distance Detector pin - ultrasonic sensor

#define LCD_ADR 0x27
#define LCD_COL 20
#define LCD_ROW 4

// TODO: Adjust pin definitions as per your hardware setup
/* ===== Distance thresholds (cm) ===== */
#define D1 100  // Distance threshold for drone exit detection
#define D2 10   // Distance threshold for drone landing detection

/* ===== Time thresholds (milliseconds) ===== */
#define T1 3000  // Time to confirm drone has exited (3 seconds)
#define T2 3000  // Time to confirm drone has landed (3 seconds)
#define T3 5000  // Time to enter pre-alarm state (5 seconds)
#define T4 5000  // Time to enter alarm state (5 seconds)

/* ===== LED blink periods ===== */
// LED blink period (milliseconds)
#define L2_BLINK_PERIOD 500  // L2 blinks with period 0.5 second during take-off/landing

// Temperature thresholds (Celsius)
#define TEMP1 30  // Pre-alarm temperature threshold
#define TEMP2 40  // Alarm temperature threshold

#endif
