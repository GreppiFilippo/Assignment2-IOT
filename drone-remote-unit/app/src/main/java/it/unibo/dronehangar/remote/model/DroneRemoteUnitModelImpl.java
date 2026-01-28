package it.unibo.dronehangar.remote.model;

import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.Optional;

import it.unibo.dronehangar.remote.api.Command;
import it.unibo.dronehangar.remote.api.ConnectionState;
import it.unibo.dronehangar.remote.api.DroneRemoteUnitModel;
import it.unibo.dronehangar.remote.api.DroneState;
import it.unibo.dronehangar.remote.api.HangarState;

/**
 * Pure model implementation without any UI dependencies.
 * All setters are thread-safe.
 */
public final class DroneRemoteUnitModelImpl implements DroneRemoteUnitModel {

    private static final String STATE_BLANK_ERROR = "State cannot be blank";

    private final List<Command> availableCommands;
    private volatile Optional<DroneState> droneState;
    private volatile Optional<HangarState> hangarState;
    private volatile Optional<Float> distance;
    private volatile ConnectionState connectionState;

    /**
     * Constructor for DroneRemoteUnitModelImpl.
     *
     * @param availableCommands the list of available commands
     */
    public DroneRemoteUnitModelImpl(final List<Command> availableCommands) {
        this.availableCommands = new ArrayList<>(availableCommands);
        this.distance = Optional.empty();
        this.droneState = Optional.empty();
        this.hangarState = Optional.empty();
        this.connectionState = ConnectionState.DISCONNECTED;
    }

    @Override
    public List<Command> getAvailableCommands() {
        return new ArrayList<>(this.availableCommands);
    }

    @Override
    public void setDistance(final float dist) {
        this.distance = Optional.of(dist);
    }

    @Override
    public void setConnectionState(final String state) {
        Objects.requireNonNull(state);
        if (state.isBlank()) {
            throw new IllegalArgumentException(STATE_BLANK_ERROR);
        }

        boolean validState = false;
        for (final var cs : ConnectionState.values()) {
            if (cs.name().equals(state)) {
                validState = true;
                break;
            }
        }

        if (!validState) {
            throw new IllegalArgumentException("Invalid connection state: " + state);
        }
        this.connectionState = ConnectionState.valueOf(state);

    }

    @Override
    public ConnectionState getConnectionState() {
        return this.connectionState;
    }

    @Override
    public Optional<DroneState> getDroneState() {
        return this.droneState;
    }

    @Override
    public Optional<HangarState> getHangarState() {
        return this.hangarState;
    }

    @Override
    public Optional<Float> getDistance() {
        return this.distance;
    }

    @Override
    public void setDroneState(final String state) {
        Objects.requireNonNull(state);
        if ("--".equals(state)) {
            this.droneState = Optional.empty();
            return;
        }
        if (state.isBlank()) {
            throw new IllegalArgumentException(STATE_BLANK_ERROR);
        }
        boolean validState = false;
        for (final var ds : DroneState.values()) {
            if (ds.name().equals(state)) {
                validState = true;
                break;
            }
        }
        if (!validState) {
            throw new IllegalArgumentException("Invalid drone state: " + state);
        }
        this.droneState = Optional.of(DroneState.valueOf(state));
    }

    @Override
    public void setHangarState(final String state) {
        Objects.requireNonNull(state);
        if ("--".equals(state)) {
            this.hangarState = Optional.empty();
            return;
        }
        if (state.isBlank()) {
            throw new IllegalArgumentException(STATE_BLANK_ERROR);
        }
        boolean validState = false;
        for (final var hs : HangarState.values()) {
            if (hs.name().equals(state)) {
                validState = true;
                break;
            }
        }
        if (!validState) {
            throw new IllegalArgumentException("Invalid hangar state: " + state);
        }
        this.hangarState = Optional.of(HangarState.valueOf(state));
    }
}
