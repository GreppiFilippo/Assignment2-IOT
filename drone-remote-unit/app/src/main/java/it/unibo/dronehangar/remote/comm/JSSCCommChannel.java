package it.unibo.dronehangar.remote.comm;

import java.util.Arrays;
import java.util.List;
import java.util.Objects;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

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
    private static final int QUEUE_SIZE = 100;
    private SerialPort serialPort;
    private int baudRate;
    private final BlockingQueue<String> queue;
    private StringBuffer currentMsg = new StringBuffer("");
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
    }

    @Override
    public void sendMsg(final String msg) {
        Objects.requireNonNull(msg, "Message cannot be null");
        Objects.requireNonNull(this.serialPort, "Serial port cannot be null");
        Objects.requireNonNull(this.baudRate, "Baud rate cannot be null");

        final char[] array = (msg + "\n").toCharArray();
        final byte[] bytes = new byte[array.length];
        for (int i = 0; i < array.length; i++) {
            bytes[i] = (byte) array[i];
        }
        try {
            synchronized (this.serialPort) {
            this.serialPort.writeBytes(bytes);
        }
        } catch (final SerialPortException ex) {
            ex.printStackTrace();
        }
    }

    @Override
    public String receiveMsg() {
        // TODO: fix
        try {
            return queue.take();
        } catch (final InterruptedException e) {
            return null;
        }
    }

    @Override
    public boolean isMsgAvailable() {
        return !this.queue.isEmpty();
    }

    @Override
    public void setCommPort(final String commPort) {
        close();
        try {
            this.serialPort = new SerialPort(commPort);
            this.serialPort.openPort();

            this.serialPort.setParams(baudRate,
                    SerialPort.DATABITS_8,
                    SerialPort.STOPBITS_1,
                    SerialPort.PARITY_NONE);

            this.serialPort.setFlowControlMode(SerialPort.FLOWCONTROL_RTSCTS_IN 
                    | SerialPort.FLOWCONTROL_RTSCTS_OUT);

            // serialPort.addEventListener(this, SerialPort.MASK_RXCHAR);
            this.serialPort.addEventListener(this);
        } catch (final Exception ex) {
            ex.printStackTrace();
        }
    }

    @Override
    public List<String> getAvailableCommPorts() {
       return Arrays.asList(SerialPortList.getPortNames());
    }

    @Override
    public List<String> getSupportedBaudRates() {
        return this.baudRates;
    }

    @Override
    public void setBaudRate(final int baudRate) {
        this.baudRate = baudRate;
    }

    @Override
    public void serialEvent(final SerialPortEvent serialPortEvent) {
        if (serialPortEvent.isRXCHAR()) {
            try {
                String msg = serialPort.readString(serialPortEvent.getEventValue());
                msg = msg.replaceAll("\r", "");
                this.currentMsg.append(msg);
                boolean goAhead = true;
                while (goAhead) {
                    final String msg2 = currentMsg.toString();
                    final int index = msg2.indexOf('\n');

                    if (index >= 0) {
                        queue.put(msg2.substring(0, index));
                        this.currentMsg = new StringBuffer("");

                        if (index + 1 < msg2.length()) {
                            this.currentMsg.append(msg2.substring(index + 1)); 
                        }
                    } else {
                        goAhead = false;
                    }
                }
            } catch (final Exception ex) {
                ex.printStackTrace();
                System.out.println("Error in receiving string from COM-port: " + ex);
            }
        }
    }

    private void close() {
        if (this.serialPort != null) {
            try {
                this.serialPort.removeEventListener();
            } catch (final SerialPortException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }

            if (this.serialPort.isOpened()) {
                try {
                    this.serialPort.closePort();
                } catch (final SerialPortException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            }
        }
    }
}
