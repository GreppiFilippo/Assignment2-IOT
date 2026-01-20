package it.unibo.dronehangar.remote.controller;

import java.util.Locale;
import java.util.concurrent.atomic.AtomicReference;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import it.unibo.dronehangar.remote.api.CommChannel;
import it.unibo.dronehangar.remote.api.DroneRemoteUnitController;
import it.unibo.dronehangar.remote.api.DroneRemoteUnitModel;
import it.unibo.dronehangar.remote.comm.JSSCCommChannel;
import it.unibo.dronehangar.remote.model.DroneRemoteUnitModelImpl;
import javafx.application.Platform;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Label;
import javafx.scene.layout.TilePane;

/**
 * Controller for the drone remote unit.
 */
public final class DroneRemoteUnitControllerImpl implements DroneRemoteUnitController {

    private static final Logger LOGGER = LoggerFactory.getLogger(DroneRemoteUnitControllerImpl.class);
    private static final int HANDSHAKE_TIMEOUT_MS = 5000;
    private static final int HANDSHAKE_DELAY_MS = 2000;

    private final CommChannel channel;
    private final DroneRemoteUnitViewModel viewModel;
    private final AtomicReference<Thread> connectionThread = new AtomicReference<>();
    private final AtomicReference<Thread> timeoutThread = new AtomicReference<>();
    private volatile boolean waitingForHandshake;

    @FXML
    private ComboBox<String> serialComboBox;
    @FXML
    private ComboBox<String> baudComboBox;
    @FXML
    private TilePane buttonBox;
    @FXML
    private Label lblDroneState;
    @FXML
    private Label lblHangarState;
    @FXML
    private Label lblConnectionStatus;
    @FXML
    private Label lblDistance;
    @FXML
    private Label lblErrorMessage;

    /**
     * Constructor for DroneRemoteUnitControllerImpl.
     *
     * @param model the drone remote unit model to use
     */
    public DroneRemoteUnitControllerImpl(final DroneRemoteUnitModel model) {
        this.channel = new JSSCCommChannel();
        this.viewModel = new DroneRemoteUnitViewModel((DroneRemoteUnitModelImpl) model);
        LOGGER.info("DroneRemoteUnitController initialized");
    }

    @FXML
    // @SuppressWarnings("javafx:useFXML") // TODO: Fix false positive
    private void initialize() {
        /*
         * Baud Rate Selection
         */
        for (final var baudRate : this.channel.getSupportedBaudRates()) {
            this.baudComboBox.getItems().add(baudRate);
        }
        // Select default initial baud rate (should be 9600)
        if (!this.baudComboBox.getItems().isEmpty()) {
            this.baudComboBox.setValue(this.baudComboBox.getItems().get(0));
            LOGGER.info("Default baud rate selected in UI: {}", this.baudComboBox.getValue());
        }
        this.baudComboBox.setOnAction(event -> {
            final String selectedBaud = this.baudComboBox.getValue();
            if (selectedBaud != null) {
                this.channel.setBaudRate(Integer.parseInt(selectedBaud));
                LOGGER.info("Baud rate set to: {}", selectedBaud);
            }
        });
        /*
         * Command Buttons
         */
        for (final var cmd : this.viewModel.getModel().getAvailableCommands()) {
            final var btn = new Button(cmd.getName().toUpperCase(Locale.ROOT));
            btn.setOnAction(event -> {
                if (!viewModel.connectionStatusProperty().get().equals("CONNECTED")) {
                    LOGGER.warn("Cannot send command: not connected");
                    showErrorMessage("Not connected. Please connect to a serial port first.");
                    return;
                }
                try {
                    LOGGER.info("Sending command: {}", cmd.getName());
                    channel.sendMsg(cmd.getName().toUpperCase(Locale.ROOT));
                    clearErrorMessage();
                } catch (final RuntimeException e) {
                    LOGGER.error("Failed to send command: {}", cmd.getName(), e);
                    showErrorMessage("Failed to send command: " + e.getMessage());
                }
            });
            final double tileW = this.buttonBox.getPrefTileWidth();
            final double tileH = this.buttonBox.getPrefTileHeight();
            if (tileW > 0) {
                btn.setPrefWidth(tileW);
            }
            if (tileH > 0) {
                btn.setPrefHeight(tileH);
            }
            this.buttonBox.getChildren().add(btn);
        }
        /*
         * Serial Port Selection
         */
        // Allow user to type a custom port path (e.g. /tmp/ttyV1)
        this.serialComboBox.setEditable(true);

        this.serialComboBox.setOnAction(event -> {
            final String selectedPort = this.serialComboBox.getValue();
            if (selectedPort != null) {
                // Cancel any existing connection attempt
                cancelConnectionThreads();

                clearErrorMessage();
                LOGGER.info("Attempting to connect to port: {}", selectedPort);
                viewModel.setConnectionStatus("CONNECTING...");
                waitingForHandshake = false;

                final Thread newThread = new Thread(() -> {
                    try {
                        LOGGER.debug("Opening port: {}", selectedPort);
                        channel.setCommPort(selectedPort);
                        waitingForHandshake = true;
                        startHandshakeTimeout();

                        Thread.sleep(HANDSHAKE_DELAY_MS);
                        channel.sendMsg("HELLO");
                        LOGGER.info("Handshake message sent");
                    } catch (final InterruptedException e) {
                        LOGGER.warn("Connection thread interrupted");
                        Thread.currentThread().interrupt();
                        Platform.runLater(() -> {
                            if (Thread.currentThread().equals(connectionThread.get())) {
                                viewModel.setConnectionStatus("CANCELLED");
                                waitingForHandshake = false;
                                showErrorMessage("Connection cancelled");
                            }
                        });
                    } catch (final Exception e) {
                        LOGGER.error("Failed to connect: {}", selectedPort, e);
                        Platform.runLater(() -> {
                            if (Thread.currentThread().equals(connectionThread.get())) {
                                viewModel.setConnectionStatus("ERROR");
                                waitingForHandshake = false;
                                showErrorMessage(e.getMessage());
                            }
                        });
                    }
                }, "Connection-Thread");

                connectionThread.set(newThread);
                newThread.start();
            }
        });
        this.updateSerialPorts();
        /*
         * Label Bindings
         */
        lblDroneState.textProperty().bind(viewModel.droneStateProperty());
        lblHangarState.textProperty().bind(viewModel.hangarStateProperty());
        lblConnectionStatus.textProperty().bind(viewModel.connectionStatusProperty());
        lblDistance.textProperty().bind(viewModel.distanceProperty());

        /*
         * Start message listener thread
         */
        startMessageListener();
    }

    private void startMessageListener() {
        final Thread listenerThread = new Thread(() -> {
            LOGGER.info("Message listener thread started");
            while (true) {
                try {
                    if (channel.isMsgAvailable()) {
                        final String msg = channel.receiveMsg();
                        if (msg != null) {
                            LOGGER.debug("Received message: {}", msg);
                            processMessage(msg);
                        }
                    }
                    Thread.sleep(100);
                } catch (final InterruptedException e) {
                    LOGGER.info("Message listener thread interrupted");
                    Thread.currentThread().interrupt();
                    break;
                }
            }
        });
        listenerThread.setDaemon(true);
        listenerThread.start();
    }

    private void processMessage(final String msg) {
        Platform.runLater(() -> {
            if ("READY".equals(msg.trim()) && waitingForHandshake) {
                waitingForHandshake = false;
                cancelTimeoutThread();
                viewModel.setConnectionStatus("CONNECTED");
                clearErrorMessage();
                LOGGER.info("Connection established");
                return;
            }

            final String[] parts = msg.split(":");
            if (parts.length >= 2) {
                final String key = parts[0].trim();
                final String value = parts[1].trim();

                switch (key) {
                    case "DRONE_STATE":
                        viewModel.setDroneState(value);
                        break;
                    case "HANGAR_STATE":
                        viewModel.setHangarState(value);
                        break;
                    case "DISTANCE":
                        viewModel.setDistance(value + " cm");
                        break;
                    case "CONNECTION":
                        viewModel.setConnectionStatus(value);
                        break;
                    default:
                        LOGGER.warn("Unknown message key: {}", key);
                        break;
                }
            }
        });
    }

    private void updateSerialPorts() {
        this.serialComboBox.getItems().clear();
        this.serialComboBox.getItems().addAll(
                this.channel.getAvailableCommPorts());
        LOGGER.info("Serial ports updated: {} ports found", this.serialComboBox.getItems().size());
    }

    private void cancelConnectionThreads() {
        cancelTimeoutThread();
        final Thread oldThread = connectionThread.getAndSet(null);
        if (oldThread != null && oldThread.isAlive()) {
            LOGGER.debug("Interrupting previous connection thread");
            oldThread.interrupt();
        }
    }

    private void cancelTimeoutThread() {
        final Thread oldTimeout = timeoutThread.getAndSet(null);
        if (oldTimeout != null && oldTimeout.isAlive()) {
            LOGGER.debug("Interrupting previous timeout thread");
            oldTimeout.interrupt();
        }
    }

    private void startHandshakeTimeout() {
        cancelTimeoutThread();

        final Thread newTimeout = new Thread(() -> {
            try {
                Thread.sleep(HANDSHAKE_TIMEOUT_MS);
                if (waitingForHandshake && Thread.currentThread().equals(timeoutThread.get())) {
                    LOGGER.warn("Handshake timeout");
                    Platform.runLater(() -> {
                        waitingForHandshake = false;
                        viewModel.setConnectionStatus("TIMEOUT");
                        showErrorMessage("Connection timeout");
                    });
                }
            } catch (final InterruptedException e) {
                LOGGER.debug("Timeout thread interrupted");
                Thread.currentThread().interrupt();
            }
        }, "Timeout-Thread");

        timeoutThread.set(newTimeout);
        newTimeout.start();
    }

    private void showErrorMessage(final String message) {
        Platform.runLater(() -> {
            lblErrorMessage.setText(message);
            lblErrorMessage.setVisible(true);
        });
    }

    private void clearErrorMessage() {
        Platform.runLater(() -> {
            lblErrorMessage.setText("");
            lblErrorMessage.setVisible(false);
        });
    }
}
