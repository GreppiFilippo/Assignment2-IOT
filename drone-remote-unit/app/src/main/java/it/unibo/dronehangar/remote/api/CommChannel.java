package it.unibo.dronehangar.remote.api;

import java.util.List;

/**
 * Communication channel interface.
 */
public interface CommChannel {

    /**
     * Send a message represented by a string (without new line).
     * 
     * <p>
     * Asynchronous model.
     * </p>
     * 
     * @param msg the message to send
     */
    void sendMsg(String msg);

    /**
     * Receive a message.
     * 
     * <p>
     * Blocking behaviour.
     * </p>
     * 
     * @return the received message
     * @throws InterruptedException if the thread is interrupted while waiting
     */
    String receiveMsg() throws InterruptedException;

    /**
     * Poll for a message with timeout.
     *
     * @param timeoutMillis timeout in milliseconds
     * @return the received message or null if timeout
     */
    String pollMsg(long timeoutMillis);

    /**
     * Check if a message is available to be received.
     * 
     * @return true if a message is available, false otherwise
     */
    boolean isMsgAvailable();

    /**
     * Set the communication port.
     *
     * @param commPort the communication port to set
     * @throws NullPointerException if commPort is null
     */
    void setCommPort(String commPort);

    /**
     * Get a list of available communication ports.
     *
     * @return a list of available communication ports
     */
    List<String> getAvailableCommPorts();

    /**
     * Get a list of supported baud rates.
     * 
     * @return a list of supported baud rates
     */
    List<String> getSupportedBaudRates();

    /**
     * Set the baud rate for the communication.
     * 
     * @param baudRate the baud rate to set
     */
    void setBaudRate(int baudRate);

    /**
     * Check if the serial port is currently open.
     * 
     * @return true if the port is open, false otherwise
     */
    boolean isPortOpen();

    /**
     * Close the communication port.
     */
    void close();
}
