#ifndef __CONFIG__
#define __CONFIG__

// Pin definitions
#define BT_PIN 2
#define LED_PIN 13
#define MOTOR_PIN 9

// Distance thresholds (cm)
#define D1 100  // Distance threshold for drone exit detection
#define D2 10   // Distance threshold for drone landing detection

// Time thresholds (milliseconds)
#define T1 3000  // Time to confirm drone has exited (3 seconds)
#define T2 3000  // Time to confirm drone has landed (3 seconds)
#define T3 5000  // Time to enter pre-alarm state (5 seconds)
#define T4 5000  // Time to enter alarm state (5 seconds)

// LED blink period (milliseconds)
#define L2_BLINK_PERIOD 500  // L2 blinks with period 0.5 second during take-off/landing

// Temperature thresholds (Celsius)
#define TEMP1 30  // Pre-alarm temperature threshold
#define TEMP2 40  // Alarm temperature threshold

#endif
