package it.unibo.dronehangar.remote.api;

/**
 * Enumeration representing the connection status.
 */
public enum ConnectionState {
    /** Connection established successfully. */
    CONNECTED,
    /** Connection is not currently established. */
    DISCONNECTED,
    /** Connection attempt is in progress. */
    CONNECTING,
    /** Connection attempt timed out. */
    TIMEOUT,
    /** An error occurred with the connection. */
    ERROR,
    /** Connection attempt or operation was cancelled. */
    CANCELLED
}
