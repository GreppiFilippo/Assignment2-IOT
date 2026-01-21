package it.unibo.dronehangar.remote.api;

/**
 * Exception thrown when serial communication fails.
 */
public class SerialCommunicationException extends Exception {

    private static final long serialVersionUID = 1L;

    /**
     * Constructor with message.
     *
     * @param message the error message
     */
    public SerialCommunicationException(final String message) {
        super(message);
    }

    /**
     * Constructor with message and cause.
     *
     * @param message the error message
     * @param cause the cause of the exception
     */
    public SerialCommunicationException(final String message, final Throwable cause) {
        super(message, cause);
    }
}
