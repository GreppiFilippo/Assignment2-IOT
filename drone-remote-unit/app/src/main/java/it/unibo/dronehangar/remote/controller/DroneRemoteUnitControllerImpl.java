package it.unibo.dronehangar.remote.controller;

import java.util.Locale;
import java.util.concurrent.atomic.AtomicReference;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.gson.Gson;
import com.google.gson.JsonObject;
import com.google.gson.JsonSyntaxException;

import it.unibo.dronehangar.remote.api.CommChannel;
import it.unibo.dronehangar.remote.api.ConnectionState;
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
    private static final int MESSAGE_POLL_TIMEOUT_MS = 100;
    private static final int MESSAGE_POLL_SLEEP_MS = 50;
    private static final double EPSILON = 1e-6;

    private static final String CSS_LABEL_SUCCESS = "label-success";
    private static final String CSS_LABEL_ERROR = "label-error";
    private static final String CSS_LABEL_MUTED = "label-muted";
    private static final String CSS_LABEL_INFO = "label-info";
    private static final String CSS_LABEL_NORMAL = "label-normal";
    private static final String CSS_HANGAR_NORMAL = "hangar-normal";
    private static final String CSS_HANGAR_ALERT = "hangar-alert";

    private static final String COLOR_GREEN = "#0fd26a";
    private static final String COLOR_RED = "#ff4d4d";

    private static final String MSG_NOT_CONNECTED = "Not connected. Please connect to a serial port first.";

    private final CommChannel channel;
    private final DroneRemoteUnitViewModel viewModel;
    private final AtomicReference<Thread> connectionThread = new AtomicReference<>();
    private final AtomicReference<Thread> listenerThread = new AtomicReference<>();
    private final Gson gson = new Gson();
    private volatile boolean running = true;

    @FXML
    private ComboBox<String> serialComboBox;
    @FXML
    private Button btnRefreshPorts;
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
    @SuppressWarnings("PMD.UnusedPrivateMethod")
    private void initialize() {
        for (final var baudRate : this.channel.getSupportedBaudRates()) {
            this.baudComboBox.getItems().add(baudRate);
        }
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

        for (final var cmd : this.viewModel.getModel().getAvailableCommands()) {
            final var btn = new Button(cmd.getName().toUpperCase(Locale.ROOT));
            btn.setOnAction(event -> {
                if (!ConnectionState.CONNECTED.name().equals(viewModel.connectionStatusProperty().get())) {
                    LOGGER.warn("Cannot send command: not connected");
                    showErrorMessage(MSG_NOT_CONNECTED);
                    return;
                }
                try {
                    LOGGER.info("Sending command: {}", cmd.getName());
                    // Send command as JSON with "command" key
                    JsonObject jsonCmd = new JsonObject();
                    jsonCmd.addProperty("command", cmd.getName().toUpperCase(Locale.ROOT));
                    channel.sendMsg(gson.toJson(jsonCmd));
                    clearErrorMessage();
                } catch (final IllegalStateException e) {
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

        this.serialComboBox.setEditable(true);

        this.serialComboBox.setOnAction(event -> {
            final String selectedPort = this.serialComboBox.getValue();
            if (selectedPort != null) {
                cancelConnectionThreads();

                clearErrorMessage();
                LOGGER.info("Attempting to connect to port: {}", selectedPort);
                viewModel.setConnectionStatus(ConnectionState.CONNECTING.name());

                final Thread newThread = new Thread(() -> {
                    try {
                        LOGGER.debug("Opening port: {}", selectedPort);
                        channel.setCommPort(selectedPort);
                        
                        // No handshake - directly mark as connected
                        Thread.sleep(500); // Small delay for port to stabilize
                        Platform.runLater(() -> {
                            viewModel.setConnectionStatus(ConnectionState.CONNECTED.name());
                            clearErrorMessage();
                            LOGGER.info("Connection established (no handshake)");
                        });
                    } catch (final InterruptedException e) {
                        LOGGER.warn("Connection thread interrupted");
                        Thread.currentThread().interrupt();
                        Platform.runLater(() -> {
                            if (Thread.currentThread().equals(connectionThread.get())) {
                                viewModel.setConnectionStatus(ConnectionState.CANCELLED.name());
                                showErrorMessage("Connection cancelled");
                            }
                        });
                    } catch (final IllegalStateException e) {
                        LOGGER.error("Failed to connect: {}", selectedPort, e);
                        Platform.runLater(() -> {
                            if (Thread.currentThread().equals(connectionThread.get())) {
                                viewModel.setConnectionStatus(ConnectionState.ERROR.name());
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
        if (this.btnRefreshPorts != null) {
            this.btnRefreshPorts.setOnAction(ev -> updateSerialPorts());
        }

        lblDroneState.textProperty().bind(viewModel.droneStateProperty());
        lblHangarState.textProperty().bind(viewModel.hangarStateProperty());
        lblConnectionStatus.textProperty().bind(viewModel.connectionStatusProperty());
        lblDistance.textProperty().bind(viewModel.distanceProperty());

        viewModel.connectionStatusProperty().addListener((obs, oldVal, newVal) -> {
            if (newVal == null) {
                return;
            }
            Platform.runLater(() -> {
                lblConnectionStatus.getStyleClass().removeAll(CSS_LABEL_SUCCESS, CSS_LABEL_ERROR, CSS_LABEL_MUTED);
                if (ConnectionState.CONNECTED.name().equals(newVal)) {
                    lblConnectionStatus.getStyleClass().add(CSS_LABEL_SUCCESS);
                } else if (ConnectionState.DISCONNECTED.name().equals(newVal)
                        || ConnectionState.ERROR.name().equals(newVal)
                        || ConnectionState.TIMEOUT.name().equals(newVal)) {
                    lblConnectionStatus.getStyleClass().add(CSS_LABEL_ERROR);
                } else {
                    lblConnectionStatus.getStyleClass().add(CSS_LABEL_MUTED);
                }
            });
        });

        viewModel.hangarStateProperty().addListener((obs, oldVal, newVal) -> {
            if (newVal == null) {
                return;
            }
            Platform.runLater(() -> {
                lblHangarState.getStyleClass().removeAll(
                        CSS_HANGAR_NORMAL, CSS_HANGAR_ALERT, CSS_LABEL_MUTED,
                        CSS_LABEL_SUCCESS, CSS_LABEL_INFO, CSS_LABEL_ERROR, CSS_LABEL_NORMAL);
                final String n = newVal.trim().toUpperCase(Locale.ROOT);
                switch (n) {
                    case "NORMAL":
                        lblHangarState.getStyleClass().add(CSS_HANGAR_NORMAL);
                        lblHangarState.setStyle("");
                        try {
                            lblHangarState.setTextFill(javafx.scene.paint.Color.web(COLOR_GREEN));
                        } catch (final IllegalArgumentException e) {
                            LOGGER.debug("Failed to set color: {}", e.getMessage());
                        }
                        LOGGER.debug("HangarState NORMAL classes={} style={}", lblHangarState.getStyleClass(),
                                lblHangarState.getStyle());
                        break;
                    case "ALARM":
                        lblHangarState.getStyleClass().add(CSS_HANGAR_ALERT);
                        lblHangarState.setStyle("-fx-text-fill: " + COLOR_RED + ";");
                        try {
                            lblHangarState.setTextFill(javafx.scene.paint.Color.web(COLOR_RED));
                        } catch (final IllegalArgumentException e) {
                            LOGGER.debug("Failed to set color: {}", e.getMessage());
                        }
                        LOGGER.debug("HangarState ALARM classes={} style={}", lblHangarState.getStyleClass(),
                                lblHangarState.getStyle());
                        break;
                    default:
                        lblHangarState.getStyleClass().add(CSS_LABEL_MUTED);
                        break;
                }
            });
        });

        startMessageListener();
    }

    private void startMessageListener() {
        final Thread listener = new Thread(() -> {
            LOGGER.info("Message listener thread started");
            while (running) {
                try {
                    final String msg = channel.pollMsg(MESSAGE_POLL_TIMEOUT_MS);
                    if (msg != null) {
                        LOGGER.debug("Received message: {}", msg);
                        processMessage(msg);
                    }
                    Thread.sleep(MESSAGE_POLL_SLEEP_MS);
                } catch (final InterruptedException e) {
                    LOGGER.info("Message listener thread interrupted");
                    Thread.currentThread().interrupt();
                    break;
                }
            }
            LOGGER.info("Message listener thread stopped");
        });
        listener.setDaemon(true);
        listenerThread.set(listener);
        listener.start();
    }

    /**
     * Shutdown method to stop all threads gracefully.
     */
    public void shutdown() {
        LOGGER.info("Shutting down controller");
        running = false;
        cancelConnectionThreads();
        final Thread listener = listenerThread.getAndSet(null);
        if (listener != null && listener.isAlive()) {
            listener.interrupt();
        }
        channel.close();
    }

    private void processMessage(final String msg) {
        Platform.runLater(() -> {
            try {
                // Parse JSON message from Arduino
                JsonObject json = gson.fromJson(msg, JsonObject.class);
                
                LOGGER.debug("Received JSON: {}", json);
                
                // Extract fields from JSON
                if (json.has("droneState")) {
                    String droneState = json.get("droneState").getAsString().toUpperCase(Locale.ROOT);
                    if (isValidDroneState(droneState)) {
                        viewModel.setDroneState(droneState);
                    }
                }
                
                if (json.has("hangarState")) {
                    String hangarState = json.get("hangarState").getAsString().toUpperCase(Locale.ROOT);
                    if (isValidHangarState(hangarState)) {
                        viewModel.setHangarState(hangarState);
                    }
                }
                
                if (json.has("distance")) {
                    double distance = json.get("distance").getAsDouble();
                    String sanitized = sanitizeDistance(String.valueOf(distance));
                    if (sanitized != null) {
                        viewModel.setDistance(sanitized + " cm");
                    }
                }
                
                if (json.has("alarm")) {
                    boolean alarm = json.get("alarm").getAsBoolean();
                    viewModel.setHangarState(alarm ? "ALARM" : "NORMAL");
                }
                
            } catch (JsonSyntaxException e) {
                LOGGER.warn("Invalid JSON received: {} - Error: {}", msg, e.getMessage());
            } catch (Exception e) {
                LOGGER.error("Error processing message: {}", msg, e);
            }
        });
    }

    private boolean isValidDroneState(final String s) {
        try {
            it.unibo.dronehangar.remote.api.DroneState.valueOf(s);
            return true;
        } catch (final IllegalArgumentException e) {
            return false;
        }
    }

    private boolean isValidHangarState(final String s) {
        try {
            it.unibo.dronehangar.remote.api.HangarState.valueOf(s);
            return true;
        } catch (final IllegalArgumentException e) {
            return false;
        }
    }

    private String sanitizeDistance(final String s) {
        final String trimmed = s.trim();
        try {
            if (trimmed.isEmpty()) {
                return null;
            }
            final double d = Double.parseDouble(trimmed);
            if (Double.isNaN(d) || Double.isInfinite(d)) {
                return null;
            }
            if (Math.abs(d - Math.round(d)) < EPSILON) {
                return String.valueOf(Math.round(d));
            }
            return String.valueOf(d);
        } catch (final NumberFormatException e) {
            return null;
        }
    }

    private boolean isValidConnectionState(final String s) {
        try {
            ConnectionState.valueOf(s);
            return true;
        } catch (final IllegalArgumentException e) {
            return false;
        }
    }

    private void updateSerialPorts() {
        this.serialComboBox.getItems().clear();
        this.serialComboBox.getItems().addAll(
                this.channel.getAvailableCommPorts());
        LOGGER.info("Serial ports updated: {} ports found", this.serialComboBox.getItems().size());
    }

    private void cancelConnectionThreads() {
        final Thread oldThread = connectionThread.getAndSet(null);
        if (oldThread != null && oldThread.isAlive()) {
            LOGGER.debug("Interrupting previous connection thread");
            oldThread.interrupt();
        }
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
