package it.unibo.dronehangar.remote.api;

/**
 * Enumeration representing the connection status.
 */
public enum ConnectionState {
    CONNECTED,
    DISCONNECTED,
    CONNECTING,
    TIMEOUT,
    ERROR,
    CANCELLED
}
