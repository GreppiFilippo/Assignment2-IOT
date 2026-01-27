package it.unibo.dronehangar.remote.service;

import java.util.List;
import java.util.Locale;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.function.Consumer;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.gson.Gson;
import com.google.gson.JsonObject;
import com.google.gson.JsonSyntaxException;

import edu.umd.cs.findbugs.annotations.SuppressFBWarnings;
import it.unibo.dronehangar.remote.api.CommChannel;
import it.unibo.dronehangar.remote.api.ConnectionState;
import it.unibo.dronehangar.remote.api.DroneRemoteUnitViewUpdater;
import it.unibo.dronehangar.remote.comm.JSSCCommChannel;
import javafx.application.Platform;

/**
 * Service responsible for managing the serial connection to the drone.
 * 
 * <p>
 * This class handles:
 * <ul>
 * <li>Opening and closing the serial connection</li>
 * <li>Sending commands to the drone</li>
 * <li>Receiving and parsing JSON messages</li>
 * <li>Updating the JavaFX UI in a thread-safe way</li>
 * </ul>
 * </p>
 *
 * <p>
 * All I/O operations are executed on background threads.
 * UI updates are always dispatched on the JavaFX Application Thread.
 * </p>
 */
public final class DroneConnectionService {

    private static final Logger LOGGER = LoggerFactory.getLogger(DroneConnectionService.class);

    /** Maximum time to wait for the initial "alive" message (ms). */
    private static final long ALIVE_TIMEOUT_MS = 1500;

    /** Polling timeout for serial messages (ms). */
    private static final long POLL_TIMEOUT_MS = 200;

    private final CommChannel channel;

    @SuppressFBWarnings(value = "EI_EXPOSE_REP2", justification = "ViewUpdater reference is intentionally stored for UI updates")
    private final DroneRemoteUnitViewUpdater viewModel;

    /** Executor for connection and command operations. */
    private final ExecutorService connectionExecutor;

    /** Executor dedicated to the listener loop. */
    private final ExecutorService listenerExecutor;

    private final Gson gson = new Gson();

    /** Indicates whether the listener loop is running. */
    private final AtomicBoolean running = new AtomicBoolean(false);

    /** Optional callback for propagating errors to the UI. */
    private Consumer<String> errorCallback;

    /**
     * Creates a new {@code DroneConnectionService}.
     *
     * @param viewModel the ViewModel used to update the UI
     */
    public DroneConnectionService(final DroneRemoteUnitViewUpdater viewModel) {
        this.viewModel = viewModel;
        this.channel = new JSSCCommChannel();

        this.connectionExecutor = Executors.newSingleThreadExecutor(r -> {
            final Thread t = new Thread(r, "Drone-Connection-Thread");
            t.setDaemon(true);
            return t;
        });

        this.listenerExecutor = Executors.newSingleThreadExecutor(r -> {
            final Thread t = new Thread(r, "Drone-Listener-Thread");
            t.setDaemon(true);
            return t;
        });
    }

    /**
     * Sets a callback to be invoked when an error occurs.
     *
     * @param callback a consumer receiving a human-readable error message
     */
    public void setErrorCallback(final Consumer<String> callback) {
        this.errorCallback = callback;
    }

    /* ===================================================================== */
    /* PUBLIC API */
    /* ===================================================================== */

    /**
     * Returns the list of available serial communication ports.
     *
     * @return a list of port identifiers
     */
    public List<String> getAvailableCommPorts() {
        return channel.getAvailableCommPorts();
    }

    /**
     * Returns the list of supported baud rates.
     *
     * @return a list of baud rates as strings
     */
    public List<String> getSupportedBaudRates() {
        return channel.getSupportedBaudRates();
    }

    /**
     * Sets the baud rate for the communication channel.
     *
     * @param baudRate the baud rate to use
     */
    public void setBaudRate(final int baudRate) {
        channel.setBaudRate(baudRate);
    }

    /**
     * Initiates a connection to the drone on the given serial port.
     * 
     * <p>
     * The connection process is asynchronous.
     * </p>
     *
     * @param port the serial port identifier
     */
    public void connect(final String port) {
        connectionExecutor.execute(() -> doConnect(port));
    }

    /**
     * Sends a command to the drone.
     *
     * @param command the command string
     */
    public void sendCommand(final String command) {
        if (!channel.isPortOpen()) {
            notifyError("Not connected: cannot send command");
            return;
        }

        connectionExecutor.execute(() -> {
            final JsonObject json = new JsonObject();
            json.addProperty("cmd", command.toLowerCase(Locale.ROOT));
            channel.sendMsg(gson.toJson(json));
        });
    }

    /**
     * Checks whether the service is currently connected.
     *
     * @return {@code true} if the serial port is open
     */
    public boolean isConnected() {
        return channel.isPortOpen();
    }

    /**
     * Shuts down the service, stopping all background threads
     * and closing the serial channel.
     */
    public void shutdown() {
        running.set(false);
        listenerExecutor.shutdownNow();
        connectionExecutor.shutdownNow();
        channel.close();
    }

    /* ===================================================================== */
    /* CONNECTION LOGIC */
    /* ===================================================================== */

    private void doConnect(final String port) {
        if (port == null || port.isBlank()) {
            LOGGER.error("Invalid serial port: '{}'", port);
            updateConnectionState(ConnectionState.ERROR);
            notifyError("Invalid serial port");
            return;
        }

        updateConnectionState(ConnectionState.CONNECTING);

        channel.setCommPort(port);

        if (!channel.isPortOpen()) {
            LOGGER.error("Failed to open serial port {}", port);
            updateConnectionState(ConnectionState.ERROR);
            notifyError("Failed to open serial port " + port);
            return;
        }

        LOGGER.info("Serial port opened: {}", port);

        if (!waitForAlive()) {
            updateConnectionState(ConnectionState.ERROR);
            channel.close();
            return;
        }

        updateConnectionState(ConnectionState.CONNECTED);
        startListenerLoop();
    }

    private boolean waitForAlive() {
        final long deadline = System.currentTimeMillis() + ALIVE_TIMEOUT_MS;

        while (System.currentTimeMillis() < deadline) {
            final String msg = channel.pollMsg(POLL_TIMEOUT_MS);
            if (msg == null) {
                continue;
            }

            try {
                final JsonObject json = gson.fromJson(msg, JsonObject.class);
                if (json.has("alive") && json.get("alive").getAsBoolean()) {
                    LOGGER.info("Alive message received");
                    return true;
                }
            } catch (final JsonSyntaxException e) {
                LOGGER.warn("Invalid JSON while waiting for alive: {}", msg);
            }
        }

        notifyError("Drone did not respond in time (alive timeout)");
        return false;
    }

    /* ===================================================================== */
    /* LISTENER LOOP */
    /* ===================================================================== */

    private void startListenerLoop() {
        running.set(true);

        listenerExecutor.execute(() -> {
            LOGGER.info("Listener thread started");

            try {
                while (running.get() && channel.isPortOpen()) {
                    final String msg = channel.pollMsg(POLL_TIMEOUT_MS);
                    if (msg != null) {
                        handleMessage(msg);
                    }
                }
            } catch (IllegalStateException | IllegalArgumentException e) {
                LOGGER.error("Listener error", e);
            } finally {
                running.set(false);
                channel.close();
                updateConnectionState(ConnectionState.DISCONNECTED);
                LOGGER.info("Listener thread stopped");
            }
        });

    }

    /* ===================================================================== */
    /* MESSAGE HANDLING */
    /* ===================================================================== */

    private void handleMessage(final String msg) {
        try {
            final JsonObject json = gson.fromJson(msg, JsonObject.class);

            Platform.runLater(() -> {
                if (json.has("drone")) {
                    viewModel.setDroneState(
                            json.get("drone").getAsString().toUpperCase(Locale.ROOT));
                } else if (json.has("drone_state")) {
                    viewModel.setDroneState(
                            json.get("drone_state").getAsString().toUpperCase(Locale.ROOT));
                }

                if (json.has("distance")) {
                    viewModel.setDistance(
                            String.valueOf(json.get("distance").getAsFloat()));
                }

                if (json.has("hangar")) {
                    viewModel.setHangarState(
                            json.get("hangar").getAsString().toUpperCase(Locale.ROOT));
                } else if (json.has("hangar_state")) {
                    final boolean alarm = json.get("hangar_state").getAsBoolean();
                    viewModel.setHangarState(alarm ? "ALARM" : "NORMAL");
                }
            });

        } catch (final JsonSyntaxException e) {
            LOGGER.warn("Invalid JSON received: {}", msg);
        }
    }

    /* ===================================================================== */
    /* UTILITY METHODS */
    /* ===================================================================== */

    private void updateConnectionState(final ConnectionState state) {
        Platform.runLater(() -> viewModel.setConnectionStatus(state.name()));
    }

    private void notifyError(final String message) {
        if (errorCallback != null) {
            Platform.runLater(() -> errorCallback.accept(message));
        }
    }
}
