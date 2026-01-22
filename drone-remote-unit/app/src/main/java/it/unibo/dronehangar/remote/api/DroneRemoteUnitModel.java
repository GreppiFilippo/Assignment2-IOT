package it.unibo.dronehangar.remote.api;

import java.util.List;
import java.util.Optional;

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
     * Get the drone state.
     * 
     * @return the drone state
     */
    Optional<DroneState> getDroneState();

    /**
     * Get the hangar state.
     * 
     * @return the hangar state
     */
    Optional<HangarState> getHangarState();

    /**
     * Get the connection state.
     * 
     * @return the connection state
     */
    ConnectionState getConnectionState();

    /**
     * Get the distance.
     * 
     * @return the distance
     */
    Optional<Float> getDistance();

    /**
     * Set the drone state.
     * Thread-safe.
     * 
     * @param state the new drone state
     */
    void setDroneState(String state);

    /**
     * Set the hangar state.
     * Thread-safe.
     * 
     * @param state the new hangar state
     */
    void setHangarState(String state);

    /**
     * Set the distance value.
     * Thread-safe.
     * 
     * @param dist the new distance value
     */
    void setDistance(float dist);

    /**
     * Set the connection status.
     * Thread-safe.
     * 
     * @param state the new connection status
     */
    void setConnectionState(String state);
}
