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

    /**
     * Get the drone state property as a string.
     * 
     * @return the drone state property string
     */
    String droneStateProperty();

    /**
     * Get the hangar state property as a string.
     * 
     * @return the hangar state property string
     */
    String hangarStateProperty();

    /**
     * Get the connection status property as a string.
     * 
     * @return the connection status property string
     */
    String connectionStatusProperty();

    /**
     * Get the distance property as a string.
     * 
     * @return the distance property string
     */
    String distanceProperty();
}
