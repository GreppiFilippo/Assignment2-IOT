package it.unibo.dronehangar.remote.model;

import java.util.ArrayList;
import java.util.List;

import it.unibo.dronehangar.remote.api.Command;
import it.unibo.dronehangar.remote.api.DroneRemoteUnitModel;
import it.unibo.dronehangar.remote.api.DroneState;
import it.unibo.dronehangar.remote.api.HangarState;

/**
 * Pure model implementation without any UI dependencies.
 * All setters are thread-safe.
 */
public final class DroneRemoteUnitModelImpl implements DroneRemoteUnitModel {

    private final List<Command> availableCommands;
    private volatile String droneState;
    private volatile String hangarState;
    private volatile String distance;
    private volatile String connectionStatus;

    /**
     * Constructor for DroneRemoteUnitModelImpl.
     *
     * @param availableCommands the list of available commands
     */
    public DroneRemoteUnitModelImpl(final List<Command> availableCommands) {
        this.availableCommands = new ArrayList<>(availableCommands);
        // Start with empty/unset values until a connection is established
        this.droneState = "--";
        this.hangarState = "--";
        this.distance = "-- cm";
        this.connectionStatus = "DISCONNECTED";
    }

    @Override
    public List<Command> getAvailableCommands() {
        return new ArrayList<>(this.availableCommands);
    }

    @Override
    public String droneStateProperty() {
        return this.droneState;
    }

    @Override
    public String distanceProperty() {
        return this.distance;
    }

    @Override
    public String hangarStateProperty() {
        return this.hangarState;
    }

    @Override
    public String connectionStatusProperty() {
        return this.connectionStatus;
    }

    /**
     * Set the drone state.
     * Thread-safe.
     * 
     * @param state the new drone state
     */
    public void setDroneState(final String state) {
        this.droneState = state;
    }

    /**
     * Set the hangar state.
     * Thread-safe.
     * 
     * @param state the new hangar state
     */
    public void setHangarState(final String state) {
        this.hangarState = state;
    }

    /**
     * Set the distance value.
     * Thread-safe.
     * 
     * @param dist the new distance value
     */
    public void setDistance(final String dist) {
        this.distance = dist;
    }

    /**
     * Set the connection status.
     * Thread-safe.
     * 
     * @param status the new connection status
     */
    public void setConnectionStatus(final String status) {
        this.connectionStatus = status;
    }
}
