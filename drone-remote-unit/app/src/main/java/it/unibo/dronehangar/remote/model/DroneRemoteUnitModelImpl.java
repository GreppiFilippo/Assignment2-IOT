package it.unibo.dronehangar.remote.model;

import java.util.ArrayList;
import java.util.List;

import it.unibo.dronehangar.remote.api.Command;
import it.unibo.dronehangar.remote.api.DroneRemoteUnitModel;
import it.unibo.dronehangar.remote.api.DroneState;
import it.unibo.dronehangar.remote.api.HangarState;

/**
 * Implementation of the DroneRemoteUnitModel interface.
 */
public final class DroneRemoteUnitModelImpl implements DroneRemoteUnitModel {
    private final List<Command> availableCommands;
    private final DroneState droneState;
    private final HangarState hangarState = HangarState.NORMAL;
    private String distanceProperty;

    /**
     * Constructor for DroneRemoteUnitModelImpl.
     *
     * @param availableCommands the list of available commands
     */
    public DroneRemoteUnitModelImpl(final List<Command> availableCommands) {
        this.availableCommands = new ArrayList<>(availableCommands);
        this.droneState = DroneState.REST;
        this.distanceProperty = "--";
    }

    @Override
    public List<Command> getAvailableCommands() {
        return new ArrayList<>(this.availableCommands);
    }

    @Override
    public String droneStateProperty() {
        return this.droneState.name();
    }

    @Override
    public String distanceProperty() {
        return this.distanceProperty;
    }

    @Override
    public String hangarStateProperty() {
        return this.hangarState.name();
    }

    @Override
    public String connectionStatusProperty() {
        // TODO Auto-generated method stub
        return null;
    }
}
