# Report Assignment #02 – *Smart Drone Hangar*
This document describes the design and implementation of the *Smart Drone Hangar system developed for Assignment #02* of the Embedded Systems and Internet of Things course at the University of Bologna.

## System schema
![layout](assignment-02-schema.png)


## Video demo
You can find the video demo [here](./video.md).

### Authors

**Filippo Greppi**
- Email: <filippo.greppi2@studio.unibo.it>
- Student ID: 0001114837
 
**Marcello Spagnoli**  
- Email: <marcello.spagnoli2@studio.unibo.it>  
- Student ID: 0001117244  

---

## 1. Introduction

The **Smart Drone Hangar** is an embedded system designed to manage autonomous drone operations, including take-off, landing, and temperature monitoring. The system consists of two main subsystems:

- **Drone Hangar (Arduino)**: Controls hangar door, monitors temperature, and manages drone presence detection
- **Drone Remote Unit (PC)**: Java-based GUI application for remote drone control and monitoring

The system implements a task-based cooperative scheduler with synchronous Finite State Machines (FSMs) for robust and predictable behavior.

---

## 2. System Architecture

### 2.1 Overview

The system follows a **distributed architecture** with two communicating subsystems:

```
┌─────────────────────────┐          Serial          ┌──────────────────────────┐
│   Drone Hangar          │ ◄──────  (115200)  ────► │  Drone Remote Unit       │
│   (Arduino/C++)         │          JSON            │  (PC/Java)               │
└─────────────────────────┘                          └──────────────────────────┘
         │                                                     │
         ├─ PIR Sensor (Presence Detection)                    ├─ GUI Controls
         ├─ Sonar (Distance Measurement)                       ├─ State Visualization
         ├─ Servo Motor (Door Control)                         ├─ Command Sending
         ├─ Temperature Sensor (TMP36)                         └─ Distance Display
         ├─ LCD Display (I2C)
         ├─ LEDs (L1, L2, L3)
         └─ Reset Button
```

### 2.2 Communication Flow

The two subsystems communicate via **serial line** using JSON messages:

- **Commands**: PC → Arduino (e.g., `{"cmd": "open"}`)
- **State Updates**: Arduino → PC (periodic, every 500ms)
  - Drone state (rest, taking_off, operating, landing)
  - Hangar state (normal, pre_alarm, alarm)
  - Distance readings
  - Temperature readings

---


## 3. Software Architecture

### 3.1 Arduino Platform

The Arduino subsystem is built using **PlatformIO** and implements:

- **Kernel Layer**:
  - `Scheduler`: Cooperative task scheduler (base period: 50ms)
  - `MsgService`: Serial communication handler with JSON parsing
  - `Logger`: Debug logging facility
  - `Task`: Base class for all tasks

- **Model Layer**:
  - `Context`: Shared state container for inter-task communication
  - `HWPlatform`: Hardware abstraction layer

- **Device Layer**:
  - Hardware drivers for all sensors and actuators
  - Abstraction interfaces (Button, Light, ProximitySensor, etc.)

- **Task Layer**:
  - 7 concurrent tasks with different periods
  - FSM-based logic for state management

### 3.2 PC Platform (Java)

The Java subsystem is built using **JavaFX** and follows a **layered architecture** with clear separation of concerns:

#### **Architecture Layers**

1. **Model Layer** (`DroneRemoteUnitModelImpl`):
   - Pure domain model without UI dependencies
   - Thread-safe state management using `volatile` and `Optional`
   - Stores: drone state, hangar state, distance, connection state
   - Validates state transitions using enumerations
   - Provides immutable access to available commands

2. **ViewModel Layer** (`DroneRemoteUnitViewModel`):
   - Bridges the pure Model and JavaFX View
   - Contains JavaFX `ReadOnlyStringProperty` for UI data binding
   - Implements `DroneRemoteUnitViewUpdater` interface for service updates
   - Synchronizes Model state with View properties
   - Must be updated from JavaFX Application Thread

3. **View Layer** (FXML + Controller):
   - **FXML**: `DroneRemoteUnit.fxml` defines UI layout
   - **Controller** (`DroneRemoteUnitControllerImpl`):
     - Handles user interactions (button clicks, port selection)
     - Binds UI components to ViewModel properties
     - Delegates commands to `DroneConnectionService`
     - Creates command buttons dynamically from Model
     - Displays error messages and connection status

4. **Service Layer** (`DroneConnectionService`):
   - Manages serial connection lifecycle (connect, disconnect, shutdown)
   - **Two dedicated ExecutorService threads**:
     - `connectionExecutor`: handles connection/command operations
     - `listenerExecutor`: runs continuous message polling loop
   - Parses incoming JSON messages using Gson
   - Updates ViewModel via Platform.runLater() for thread-safety
   - Implements data freshness checking (clears stale data after timeout)
   - Waits for "alive" message during connection handshake

5. **Communication Layer** (`JSSCCommChannel`):
   - Serial communication using **JSSC library** (Java Simple Serial Connector)
   - Line-based protocol: messages delimited by `\n`
   - **Event-driven reception**: implements `SerialPortEventListener`
   - Internal `BlockingQueue` for received messages
   - Character buffer for assembling multi-chunk messages
   - Thread-safe port operations with synchronized access
   - Supports baud rate configuration (9600-115200)

---

## 4. Task-Based Implementation

### 4.1 Task Overview

The system implements **7 concurrent tasks** with cooperative scheduling:

| Task | Period (ms) | Purpose |
|------|-------------|---------|
| **DroneTask** | 50 | Main FSM controlling drone lifecycle |
| **HangarTask** | 200 | Temperature monitoring and alarm management |
| **BlinkingTask** | 500 | L2 LED blinking during operations |
| **DoorControlTask** | 50 | Smooth servo motor control |
| **DistanceTask** | 50 | Sonar monitoring with debouncing |
| **LCDTask** | 100 | LCD display updates |
| **MSGTask** | 50 | Serial communication handling |

They interact by using the Context, which holds all the shared variables

### 4.2 Scheduler Details

- **Base Period**: 50ms
- **Task Execution**: Each task's `tick()` method called at its period

---

## 5. Finite State Machines

### 5.1 DroneTask FSM

![DroneTask FSM](DroneTask.svg)


### 5.2 HangarTask FSM

![HangarTask FSM](HangarTask.svg)


### 5.3 DoorControlTask FSM

![DoorControlTask FSM](DoorControlTask.svg)

### 5.4 DistanceTask FSM
![DistanceTask FSM](DistanceTask.svg)

### 5.5 BlinkingTask FSM

![BlinkingTask FSM](BlinkingTask.svg)

---

## 6. Communication Protocol
### 6.1 Message Format

All messages use **JSON format** for clarity and extensibility.

### 6.2 Commands (PC → Arduino)

**Open Door Command**:
```json
{
  "cmd": "open"
}
```

### 6.3 State Updates (Arduino → PC)
**Periodic Status Message** (every 500ms):
```json
{
  "drone": "rest",           // "rest" | "taking_off" | "operating" | "landing"
  "hangar": "normal",        // "normal" | "pre_alarm" | "alarm"
  "distance": 0.154,          // Current distance in m
  "temperature": 25.3,       // Current temperature in °C
  "alive": true              // Heartbeat indicator
}
```

### 6.4 Command Handling

- **Queue-based**: Commands stored in Context queue
- **TTL (Time-To-Live)**: Commands expire after 5000ms
- **Priority**: FIFO (First-In-First-Out)
- **Validation**: Commands ignored during alarm state

---

**End of Report**
