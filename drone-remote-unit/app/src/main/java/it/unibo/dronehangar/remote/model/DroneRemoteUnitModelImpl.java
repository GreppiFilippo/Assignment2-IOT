package it.unibo.dronehangar.remote.model;

import java.util.ArrayList;
import java.util.List;

import it.unibo.dronehangar.remote.api.Command;
import it.unibo.dronehangar.remote.api.DroneRemoteUnitModel;

/**
 * Implementation of the DroneRemoteUnitModel interface.
 */
public final class DroneRemoteUnitModelImpl implements DroneRemoteUnitModel {
    private final List<Command> availableCommands;
    private String droneState;
    private String distanceProperty;

    /**
     * Constructor for DroneRemoteUnitModelImpl.
     *
     * @param availableCommands the list of available commands
     */
    public DroneRemoteUnitModelImpl(final List<Command> availableCommands) {
        this.availableCommands = new ArrayList<>(availableCommands);
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
        return this.distanceProperty;
    }
}
