package it.unibo.dronehangar.remote.api;

/**
 * Enumeration representing the possible states of the drone.
 */
public enum DroneState {
    /** Drone is idle on the ground. */
    REST,
    /** Drone is performing take-off. */
    TAKING_OFF,
    /** Drone is in normal operation (flying). */
    OPERATING,
    /** Drone is performing landing. */
    LANDING
}
