package it.unibo.dronehangar.remote.api;

/**
 * Enumeration representing the state of the hangar.
 */
public enum HangarState {
    /** Hangar operating in normal status. */
    NORMAL,
    /** Hangar in alarm state (attention required). */
    ALARM,
    /** Hangar in pre-alarm state (warning). */
    PRE_ALARM
}
