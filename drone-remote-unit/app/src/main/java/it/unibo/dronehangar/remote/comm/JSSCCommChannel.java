package it.unibo.dronehangar.remote.comm;

import java.util.Arrays;
import java.util.List;
import java.util.Objects;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import it.unibo.dronehangar.remote.api.CommChannel;
import jssc.SerialPort;
import jssc.SerialPortEvent;
import jssc.SerialPortEventListener;
import jssc.SerialPortException;
import jssc.SerialPortList;

/**
 * Communication channel implementation using JSSC library for serial communication.
 */
public final class JSSCCommChannel implements CommChannel, SerialPortEventListener {
    private static final Logger LOGGER = LoggerFactory.getLogger(JSSCCommChannel.class);
    private static final int QUEUE_SIZE = 100;
    private volatile SerialPort serialPort;
    private volatile int baudRate;
    private final BlockingQueue<String> queue;
    private final StringBuilder currentMsg = new StringBuilder();
    private final Object portLock = new Object();
    private final List<String> baudRates = List.of(
        "9600", 
        "19200", 
        "38400", 
        "57600", 
        "115200"
    );

    /**
     * Constructor for JSSCCommChannel.
     */
    public JSSCCommChannel() {
        this.queue = new LinkedBlockingQueue<>(QUEUE_SIZE);
        this.baudRate = Integer.parseInt(baudRates.get(0));
        LOGGER.info("JSSCCommChannel initialized with default baud rate: {}", this.baudRate);
    }

    @Override
    public void sendMsg(final String msg) {
        Objects.requireNonNull(msg, "Message cannot be null");
        
        final SerialPort port = this.serialPort;
        if (port == null) {
            throw new IllegalStateException("Serial port is not open");
        }

        LOGGER.debug("Sending message: {}", msg);
        final byte[] bytes = (msg + "\n").getBytes(java.nio.charset.StandardCharsets.UTF_8);
        
        try {
            synchronized (portLock) {
                if (port.isOpened()) {
                    port.writeBytes(bytes);
                    LOGGER.debug("Message sent successfully");
                } else {
                    throw new IllegalStateException("Serial port is not open");
                }
            }
        } catch (final SerialPortException ex) {
            LOGGER.error("Failed to send message: {}", msg, ex);
            throw new RuntimeException("Failed to send message", ex);
        }
    }

    @Override
    public String receiveMsg() {
        try {
            final String msg = queue.take();
            LOGGER.debug("Message received from queue: {}", msg);
            return msg;
        } catch (final InterruptedException e) {
            LOGGER.warn("Interrupted while waiting for message", e);
            Thread.currentThread().interrupt();
            return null;
        }
    }

    @Override
    public boolean isMsgAvailable() {
        return !this.queue.isEmpty();
    }

    @Override
    public void setCommPort(final String commPort) {
        LOGGER.info("Setting comm port to: {}", commPort);
        
        synchronized (portLock) {
            // Only close if we're already connected to a DIFFERENT port
            if (this.serialPort != null) {
                final String currentPort = this.serialPort.getPortName();
                if (!currentPort.equals(commPort)) {
                    LOGGER.debug("Switching from {} to {}, closing old port", currentPort, commPort);
                    closeInternal();
                } else {
                    LOGGER.debug("Port {} already open, closing first", currentPort);
                    closeInternal();
                }
            }
            
            try {
                this.serialPort = new SerialPort(commPort);
                this.serialPort.openPort();
                LOGGER.debug("Port {} opened successfully", commPort);

                this.serialPort.setParams(baudRate,
                        SerialPort.DATABITS_8,
                        SerialPort.STOPBITS_1,
                        SerialPort.PARITY_NONE);
                LOGGER.debug("Serial port parameters set: baudRate={}, dataBits=8, stopBits=1, parity=NONE", baudRate);

                this.serialPort.setFlowControlMode(SerialPort.FLOWCONTROL_RTSCTS_IN 
                        | SerialPort.FLOWCONTROL_RTSCTS_OUT);

                this.serialPort.addEventListener(this);
                LOGGER.info("Serial port {} configured and ready", commPort);
            } catch (final SerialPortException ex) {
                this.serialPort = null;
                LOGGER.error("Failed to configure serial port: {}", commPort, ex);
                
                final String errorMessage;
                if (ex.getMessage().contains("Port busy")) {
                    errorMessage = "Port is already in use by another application";
                } else if (ex.getMessage().contains("Port not found")) {
                    errorMessage = "Port not found or disconnected";
                } else if (ex.getMessage().contains("Permission denied")) {
                    errorMessage = "Permission denied - check device access rights";
                } else {
                    errorMessage = ex.getMessage();
                }
                
                throw new RuntimeException(errorMessage, ex);
            }
        }
    }

    @Override
    public List<String> getAvailableCommPorts() {
        final List<String> ports = Arrays.asList(SerialPortList.getPortNames());
        LOGGER.debug("Available comm ports: {}", ports);
        return ports;
    }

    @Override
    public List<String> getSupportedBaudRates() {
        return this.baudRates;
    }

    @Override
    public void setBaudRate(final int baudRate) {
        LOGGER.info("Setting baud rate to: {}", baudRate);
        this.baudRate = baudRate;
        
        // If port is already open, apply the new baud rate
        final SerialPort port = this.serialPort;
        if (port != null && port.isOpened()) {
            synchronized (portLock) {
                try {
                    port.setParams(baudRate,
                            SerialPort.DATABITS_8,
                            SerialPort.STOPBITS_1,
                            SerialPort.PARITY_NONE);
                    LOGGER.info("Baud rate updated on active connection");
                } catch (final SerialPortException e) {
                    LOGGER.error("Failed to update baud rate on active connection", e);
                }
            }
        }
    }

    @Override
    public void serialEvent(final SerialPortEvent serialPortEvent) {
        if (serialPortEvent.isRXCHAR()) {
            final SerialPort port = this.serialPort;
            if (port == null) {
                LOGGER.warn("Received serial event but port is null");
                return;
            }
            
            try {
                final String msg = port.readString(serialPortEvent.getEventValue());
                if (msg == null) {
                    return;
                }
                
                final String cleanMsg = msg.replaceAll("\r", "");
                
                synchronized (currentMsg) {
                    this.currentMsg.append(cleanMsg);
                    boolean goAhead = true;
                    while (goAhead) {
                        final String bufferedMsg = currentMsg.toString();
                        final int index = bufferedMsg.indexOf('\n');

                        if (index >= 0) {
                            final String completeMsg = bufferedMsg.substring(0, index);
                            
                            // Use offer() instead of put() to avoid blocking
                            if (queue.offer(completeMsg)) {
                                LOGGER.debug("Complete message queued: {}", completeMsg);
                            } else {
                                LOGGER.warn("Message queue full, dropping message: {}", completeMsg);
                            }
                            
                            this.currentMsg.setLength(0);
                            if (index + 1 < bufferedMsg.length()) {
                                this.currentMsg.append(bufferedMsg.substring(index + 1)); 
                            }
                        } else {
                            goAhead = false;
                        }
                    }
                }
            } catch (final SerialPortException ex) {
                LOGGER.error("Error reading from serial port", ex);
            }
        }
    }

    @Override
    public void close() {
        synchronized (portLock) {
            closeInternal();
        }
    }
    
    private void closeInternal() {
        // Must be called inside portLock synchronization
        if (this.serialPort != null) {
            final SerialPort portToClose = this.serialPort;
            final String portName = portToClose.getPortName();
            LOGGER.info("Closing serial port: {}", portName);
            
            // Set to null IMMEDIATELY
            this.serialPort = null;
            
            // Clear buffers
            synchronized (currentMsg) {
                this.currentMsg.setLength(0);
            }
            this.queue.clear();
            
            // Try to close synchronously but with error suppression
            try {
                portToClose.removeEventListener();
                LOGGER.debug("Event listener removed from {}", portName);
            } catch (final Exception e) {
                LOGGER.warn("Error removing listener: {}", e.getMessage());
            }

            try {
                if (portToClose.isOpened()) {
                    portToClose.closePort();
                    LOGGER.info("Port {} closed", portName);
                }
            } catch (final Exception e) {
                // If close fails, the port might be stuck - just log and continue
                LOGGER.warn("Error closing port {}: {} - port may be stuck", portName, e.getMessage());
            }
        }
    }
    
    @Override
    public boolean isPortOpen() {
        final SerialPort port = this.serialPort;
        return port != null && port.isOpened();
    }
}
