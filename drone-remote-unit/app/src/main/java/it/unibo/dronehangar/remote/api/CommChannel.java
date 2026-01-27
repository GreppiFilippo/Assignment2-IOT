package it.unibo.dronehangar.remote.api;

import java.util.List;

/**
 * Communication channel abstraction.
 * 
 * <p>
 * Implementations provide asynchronous message sending and
 * blocking or timed message reception.
 * </p>
 */
public interface CommChannel {

    /**
     * Sends a message represented as a string (without newline).
     *
     * <p>
     * The message is sent asynchronously.
     * </p>
     *
     * @param msg the message to send (not null)
     * @throws IllegalStateException if the communication channel is not open
     */
    void sendMsg(String msg);

    /**
     * Receives a message from the channel.
     *
     * <p>
     * This method blocks until a message is available or the thread
     * is interrupted.
     * </p>
     *
     * @return the received message, or {@code null} if the thread is interrupted
     */
    String receiveMsg();

    /**
     * Polls for a message with a timeout.
     *
     * @param timeoutMillis timeout in milliseconds
     * @return the received message, or {@code null} if the timeout expires
     *         or the thread is interrupted
     */
    String pollMsg(long timeoutMillis);

    /**
     * Checks whether a message is available.
     *
     * @return {@code true} if a message is available, {@code false} otherwise
     */
    boolean isMsgAvailable();

    /**
     * Sets the communication port.
     *
     * <p>
     * If a port is already open, it is closed before opening the new one.
     * The operation may fail silently; clients should always check
     * {@link #isPortOpen()} after calling this method.
     * </p>
     *
     * @param commPort the communication port identifier (not null)
     */
    void setCommPort(String commPort);

    /**
     * Returns the list of available communication ports.
     *
     * @return a list of available communication ports
     */
    List<String> getAvailableCommPorts();

    /**
     * Returns the list of supported baud rates.
     *
     * @return a list of supported baud rates
     */
    List<String> getSupportedBaudRates();

    /**
     * Sets the baud rate for the communication channel.
     *
     * <p>
     * If the port is already open, the new baud rate is applied immediately.
     * </p>
     *
     * @param baudRate the baud rate to set
     */
    void setBaudRate(int baudRate);

    /**
     * Checks whether the communication port is currently open.
     *
     * @return {@code true} if the port is open, {@code false} otherwise
     */
    boolean isPortOpen();

    /**
     * Closes the communication channel and releases all resources.
     */
    void close();
}
