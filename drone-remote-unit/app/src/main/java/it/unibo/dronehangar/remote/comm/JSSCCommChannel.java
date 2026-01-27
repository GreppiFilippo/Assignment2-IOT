package it.unibo.dronehangar.remote.comm;

import java.nio.charset.StandardCharsets;
import java.util.Arrays;
import java.util.List;
import java.util.Objects;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import it.unibo.dronehangar.remote.api.CommChannel;
import jssc.SerialPort;
import jssc.SerialPortEvent;
import jssc.SerialPortEventListener;
import jssc.SerialPortException;
import jssc.SerialPortList;

/**
 * Serial communication channel implementation based on JSSC.
 *
 * <p>
 * The communication protocol is line-based: each message must end with '\n'.
 * Incoming data is buffered and split into complete lines.
 * </p>
 *
 * <p>
 * This class is thread-safe.
 * </p>
 */
public final class JSSCCommChannel implements CommChannel, SerialPortEventListener {

    private static final Logger LOGGER = LoggerFactory.getLogger(JSSCCommChannel.class);

    private static final int QUEUE_SIZE = 100;
    private static final int MAX_RX_BUFFER_SIZE = 2048;

    private static final List<String> SUPPORTED_BAUD_RATES = List.of(
            "9600", "19200", "38400", "57600", "115200");
    private volatile SerialPort serialPort;

    private volatile int baudRate;
    private final BlockingQueue<String> queue;
    private final Object portLock = new Object();

    private final Object rxLock = new Object();
    private final char[] rxBuffer = new char[MAX_RX_BUFFER_SIZE];

    private int rxLen;

    /**
     * Creates a new {@code JSSCCommChannel} with default baud rate.
     */
    public JSSCCommChannel() {
        this.queue = new LinkedBlockingQueue<>(QUEUE_SIZE);
        this.baudRate = Integer.parseInt(SUPPORTED_BAUD_RATES.get(0));
        LOGGER.info("JSSCCommChannel initialized (baudRate={})", baudRate);
    }

    /* ===================================================================== */
    /* SEND */
    /* ===================================================================== */

    @Override
    public void sendMsg(final String msg) {
        Objects.requireNonNull(msg, "Message cannot be null");

        final SerialPort port = this.serialPort;
        if (port == null || !port.isOpened()) {
            throw new IllegalStateException("Serial port not open");
        }

        final byte[] bytes = (msg + "\n").getBytes(StandardCharsets.UTF_8);

        try {
            synchronized (portLock) {
                port.writeBytes(bytes);
            }
            LOGGER.debug("Sent: {}", msg);
        } catch (final SerialPortException e) {
            LOGGER.error("Failed to send message", e);
            throw new IllegalStateException("Serial write failed", e);
        }
    }

    /* ===================================================================== */
    /* RECEIVE API */
    /* ===================================================================== */

    @Override
    public String receiveMsg() {
        try {
            return queue.take();
        } catch (final InterruptedException e) {
            Thread.currentThread().interrupt();
            return null;
        }
    }

    @Override
    public String pollMsg(final long timeoutMillis) {
        try {
            return queue.poll(timeoutMillis, TimeUnit.MILLISECONDS);
        } catch (final InterruptedException e) {
            Thread.currentThread().interrupt();
            return null;
        }
    }

    @Override
    public boolean isMsgAvailable() {
        return !queue.isEmpty();
    }

    /* ===================================================================== */
    /* PORT MANAGEMENT */
    /* ===================================================================== */

    @Override
    public void setCommPort(final String commPort) {
        Objects.requireNonNull(commPort, "Communication port cannot be null");

        synchronized (portLock) {
            closeInternal();

            try {
                serialPort = new SerialPort(commPort);
                serialPort.openPort();
                serialPort.setParams(
                        baudRate,
                        SerialPort.DATABITS_8,
                        SerialPort.STOPBITS_1,
                        SerialPort.PARITY_NONE);
                serialPort.setFlowControlMode(SerialPort.FLOWCONTROL_NONE);
                serialPort.addEventListener(this);

                LOGGER.info("Serial port '{}' opened (baudRate={})", commPort, baudRate);

            } catch (final SerialPortException e) {
                serialPort = null;
                LOGGER.warn(
                        "Failed to open serial port '{}': {}",
                        commPort,
                        mapSerialError(e));
            }
        }
    }

    @Override
    public List<String> getAvailableCommPorts() {
        return Arrays.asList(SerialPortList.getPortNames());
    }

    @Override
    public List<String> getSupportedBaudRates() {
        return SUPPORTED_BAUD_RATES;
    }

    @Override
    public void setBaudRate(final int baudRate) {
        this.baudRate = baudRate;

        final SerialPort port = this.serialPort;
        if (port != null && port.isOpened()) {
            try {
                synchronized (portLock) {
                    port.setParams(
                            baudRate,
                            SerialPort.DATABITS_8,
                            SerialPort.STOPBITS_1,
                            SerialPort.PARITY_NONE);
                }
                LOGGER.info("Baud rate updated to {}", baudRate);
            } catch (final SerialPortException e) {
                LOGGER.warn("Failed to update baud rate", e);
            }
        }
    }

    @Override
    public boolean isPortOpen() {
        final SerialPort port = this.serialPort;
        return port != null && port.isOpened();
    }

    /* ===================================================================== */
    /* SERIAL EVENTS */
    /* ===================================================================== */

    @Override
    public void serialEvent(final SerialPortEvent event) {
        if (!event.isRXCHAR()) {
            return;
        }

        final SerialPort port = this.serialPort;
        if (port == null) {
            LOGGER.warn("Received data while port is closed");
            return;
        }

        try {
            final String data = port.readString(event.getEventValue());
            if (data != null && !data.isEmpty()) {
                processIncomingData(data);
            }
        } catch (final SerialPortException e) {
            LOGGER.error("Serial read error", e);
        }
    }

    /* ===================================================================== */
    /* CLOSE */
    /* ===================================================================== */

    @Override
    public void close() {
        synchronized (portLock) {
            closeInternal();
        }
    }

    private void closeInternal() {
        if (serialPort != null) {
            try {
                serialPort.removeEventListener();
            } catch (final SerialPortException ignored) {
                // ignored
            }
            try {
                if (serialPort.isOpened()) {
                    serialPort.closePort();
                }
            } catch (final SerialPortException e) {
                LOGGER.warn("Error closing serial port", e);
            } finally {
                serialPort = null;
                queue.clear();
                synchronized (rxLock) {
                    rxLen = 0;
                }
            }
        }
    }

    /* ===================================================================== */
    /* RX BUFFER + LINE PARSING */
    /* ===================================================================== */

    private void processIncomingData(final String data) {
        final String clean = data.replace("\r", "");

        synchronized (rxLock) {
            final int dataLen = clean.length();
            if (dataLen == 0) {
                return;
            }

            if (rxLen + dataLen > MAX_RX_BUFFER_SIZE) {
                LOGGER.warn("RX buffer overflow, discarding oldest data");
                final int remove = Math.max(1, rxLen / 2);
                System.arraycopy(rxBuffer, remove, rxBuffer, 0, rxLen - remove);
                rxLen -= remove;
            }

            clean.getChars(0, dataLen, rxBuffer, rxLen);
            rxLen += dataLen;

            for (int i = 0; i < rxLen; i++) {
                if (rxBuffer[i] == '\n') {
                    final String msg = new String(rxBuffer, 0, i);

                    final int remaining = rxLen - (i + 1);
                    if (remaining > 0) {
                        System.arraycopy(rxBuffer, i + 1, rxBuffer, 0, remaining);
                    }
                    rxLen = remaining;

                    if (!queue.offer(msg)) {
                        LOGGER.warn("Message queue full, dropping message: {}", msg);
                    } else {
                        LOGGER.debug("Received: {}", msg);
                    }

                    i = -1; // restart scan
                }
            }
        }
    }

    /* ===================================================================== */
    /* ERROR MAPPING */
    /* ===================================================================== */

    private String mapSerialError(final SerialPortException e) {
        final String msg = e.getMessage();
        return switch (msg) {
            case "busy" -> "Serial port busy";
            case "not found" -> "Serial port not found";
            case "Permission" -> "Permission denied";
            default -> msg;
        };
    }
}
