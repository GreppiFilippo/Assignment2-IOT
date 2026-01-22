package it.unibo.dronehangar.remote.service;

import java.util.List;
import java.util.Locale;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.function.Consumer;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.gson.Gson;
import com.google.gson.JsonObject;
import com.google.gson.JsonSyntaxException;

import it.unibo.dronehangar.remote.api.CommChannel;
import it.unibo.dronehangar.remote.api.ConnectionState;
import it.unibo.dronehangar.remote.api.DroneRemoteUnitViewUpdater;
import it.unibo.dronehangar.remote.comm.JSSCCommChannel;
import edu.umd.cs.findbugs.annotations.SuppressFBWarnings;
import javafx.application.Platform;

/**
 * Service that manages the connection to the drone via serial communication.
 * 
 * <p>
 * It handles connecting, sending commands, and receiving status updates.
 * </p>
 */
public final class DroneConnectionService {

    private static final Logger LOGGER = LoggerFactory.getLogger(DroneConnectionService.class);

    private static final long ALIVE_TIMEOUT_MS = 1500;
    private static final long POLL_TIMEOUT_MS = 200;

    private final CommChannel channel;
    @SuppressFBWarnings(value = "EI_EXPOSE_REP2", justification = "Stores updater reference for UI updates")
    private final DroneRemoteUnitViewUpdater viewModel;
    private final ExecutorService executor;
    private final Gson gson = new Gson();

    private volatile boolean running;

    // Callback per errori da mostrare nella UI
    private Consumer<String> errorCallback;

    /**
     * Constructor.
     * 
     * @param viewModel the ViewModel to update with connection status
     */
    public DroneConnectionService(final DroneRemoteUnitViewUpdater viewModel) {
        this.viewModel = viewModel;
        this.channel = new JSSCCommChannel();
        this.executor = Executors.newSingleThreadExecutor(r -> {
            final Thread t = new Thread(r, "Drone-Connection-Thread");
            t.setDaemon(true);
            return t;
        });
    }

    /**
     * Sets the error callback.
     * 
     * @param callback the callback to invoke on errors
     */
    public void setErrorCallback(final Consumer<String> callback) {
        this.errorCallback = callback;
    }

    /* ===================================================================== */
    /* PUBLIC API */
    /* ===================================================================== */

    /**
     * Gets the list of available communication ports.
     * 
     * @return list of available ports
     */
    public List<String> getAvailableCommPorts() {
        return channel.getAvailableCommPorts();
    }

    /**
     * Gets the supported baud rates.
     * 
     * @return list of supported baud rates
     */
    public List<String> getSupportedBaudRates() {
        return channel.getSupportedBaudRates();
    }

    /**
     * Sets the baud rate for the communication channel.
     * 
     * @param baudRate the baud rate to set
     */
    public void setBaudRate(final int baudRate) {
        channel.setBaudRate(baudRate);
    }

    /**
     * Connects to the drone on the specified port.
     * 
     * @param port the communication port to connect to
     */
    public void connect(final String port) {
        executor.execute(() -> doConnect(port));
    }

    /**
     * Sends a command to the drone.
     * 
     * @param command the command to send
     */
    public void sendCommand(final String command) {
        if (!channel.isPortOpen()) {
            notifyError("Not connected: cannot send command");
            return;
        }

        final JsonObject json = new JsonObject();
        json.addProperty("command", command.toUpperCase(Locale.ROOT));
        channel.sendMsg(gson.toJson(json));
    }

    /**
     * Checks if the communication channel is connected.
     * 
     * @return true if connected, false otherwise
     */
    public boolean isConnected() {
        return channel.isPortOpen();
    }

    /**
     * Shuts down the connection service, stopping all threads and closing the
     * channel.
     */
    public void shutdown() {
        running = false;
        executor.shutdownNow();
        channel.close();
    }

    /* ===================================================================== */
    /* CONNECTION LOGIC */
    /* ===================================================================== */

    private void doConnect(final String port) {
        if (port == null || port.isBlank()) {
            LOGGER.error("Connection failed: invalid port '{}'", port);
            updateConnectionState(ConnectionState.ERROR);
            notifyError("Connection failed: invalid port");
            return;
        }

        updateConnectionState(ConnectionState.CONNECTING);

        channel.setCommPort(port); // qui può fallire (implementation may log instead of throwing)
        LOGGER.info("Serial port opened: {}", port);

        if (!waitForAlive()) {
            LOGGER.error("Connection failed: no alive from drone");
            updateConnectionState(ConnectionState.ERROR);
            channel.close();
            notifyError("Connection failed: no alive received from drone");
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
                    LOGGER.info("Alive received");
                    return true;
                }
            } catch (final JsonSyntaxException ignored) {
                LOGGER.warn("Invalid JSON received while waiting for alive: {}", msg);
            }
        }

        notifyError("Drone did not respond in time (alive timeout)");
        return false;
    }

    /* ===================================================================== */
    /* LISTENER LOOP */
    /* ===================================================================== */

    private void startListenerLoop() {
        running = true;

        executor.execute(() -> {
            LOGGER.info("Message listener started");

            while (running && channel.isPortOpen()) {
                try {
                    final String msg = channel.pollMsg(POLL_TIMEOUT_MS);
                    if (msg != null) {
                        handleMessage(msg);
                    }
                } catch (IllegalStateException | IllegalArgumentException e) {
                    LOGGER.error("Listener error", e);
                    break;
                }
            }

            LOGGER.info("Message listener stopped");
            updateConnectionState(ConnectionState.DISCONNECTED);
        });
    }

    /* ===================================================================== */
    /* MESSAGE HANDLING */
    /* ===================================================================== */

    private void handleMessage(final String msg) {
        try {
            final JsonObject json = gson.fromJson(msg, JsonObject.class);

            Platform.runLater(() -> {
                if (json.has("drone_state")) {
                    viewModel.setDroneState(json.get("drone_state").getAsString().toUpperCase(Locale.ROOT));
                }

                if (json.has("distance")) {
                    viewModel.setDistance(String.valueOf(json.get("distance").getAsFloat()));
                }

                if (json.has("hangar_state")) {
                    final boolean state = json.get("hangar_state").getAsBoolean();
                    viewModel.setHangarState(state ? "ALARM" : "NORMAL");
                }
            });

        } catch (final JsonSyntaxException e) {
            LOGGER.warn("Invalid JSON received: {}", msg);
        } catch (final IllegalStateException | IllegalArgumentException e) {
            LOGGER.error("Error processing message: {}", msg, e);
        }
    }

    /* ===================================================================== */
    /* UTILS */
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
