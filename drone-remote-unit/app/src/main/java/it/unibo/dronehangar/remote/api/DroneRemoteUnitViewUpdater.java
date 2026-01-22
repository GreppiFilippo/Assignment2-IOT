package it.unibo.dronehangar.remote.api;

/**
 * Minimal interface exposing ViewModel update methods required by services.
 * This avoids exposing the full ViewModel implementation to other components.
 */
public interface DroneRemoteUnitViewUpdater {
    /**
     * Update the drone state shown in the UI.
     *
     * @param state the new drone state
     */
    void setDroneState(String state);

    /**
     * Update the hangar state shown in the UI.
     *
     * @param state the new hangar state
     */
    void setHangarState(String state);

    /**
     * Update the distance shown in the UI.
     *
     * @param dist the new distance
     */
    void setDistance(String dist);

    /**
     * Update the connection status shown in the UI.
     *
     * @param status the new connection status
     */
    void setConnectionStatus(String status);
}
