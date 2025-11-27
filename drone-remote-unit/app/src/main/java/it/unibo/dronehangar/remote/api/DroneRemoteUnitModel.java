package it.unibo.dronehangar.remote.api;

import java.util.List;

/**
 * Interface representing the model of a drone remote unit.
 */
public interface DroneRemoteUnitModel {

    /**
     * Get the available commands as a list of commands.
     * 
     * @return the list of available commands
     */
    List<Command> getAvailableCommands();

    String droneStateProperty();

    String distanceProperty();
}
