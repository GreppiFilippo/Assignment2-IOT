package it.unibo.dronehangar.remote.viewmodel;

import it.unibo.dronehangar.remote.api.DroneRemoteUnitModel;
import it.unibo.dronehangar.remote.api.DroneRemoteUnitViewUpdater;
import edu.umd.cs.findbugs.annotations.SuppressFBWarnings;
import javafx.beans.property.ReadOnlyStringProperty;
import javafx.beans.property.ReadOnlyStringWrapper;

/**
 * ViewModel that bridges between pure Model and JavaFX View.
 * Contains JavaFX properties for UI binding.
 */
@SuppressFBWarnings(value = "EI_EXPOSE_REP2", justification = "Model is shared and updated internally; not exposed directly")
public final class DroneRemoteUnitViewModel implements DroneRemoteUnitViewUpdater {
    private final DroneRemoteUnitModel model;
    private final ReadOnlyStringWrapper droneState;
    private final ReadOnlyStringWrapper hangarState;
    private final ReadOnlyStringWrapper distance;
    private final ReadOnlyStringWrapper connectionStatus;

    /**
     * Constructor.
     * 
     * @param model the pure model
     */
    public DroneRemoteUnitViewModel(final DroneRemoteUnitModel model) {
        this.model = model;
        this.droneState = new ReadOnlyStringWrapper(
                model.getDroneState().isEmpty() ? "--" : model.getDroneState().get().name());
        this.hangarState = new ReadOnlyStringWrapper(
                model.getHangarState().isEmpty() ? "--" : model.getHangarState().get().name());
        this.distance = new ReadOnlyStringWrapper(
                String.valueOf(model.getDistance().isEmpty() ? "--" : model.getDistance().get()));
        this.connectionStatus = new ReadOnlyStringWrapper(model.getConnectionState().name());
    }

    /**
     * Update drone state in both model and view property.
     * Must be called from JavaFX Application Thread.
     * 
     * @param state the new state
     */
    @Override
    public void setDroneState(final String state) {
        model.setDroneState(state);
        droneState.set(state);
    }

    /**
     * Update hangar state in both model and view property.
     * Must be called from JavaFX Application Thread.
     * 
     * @param state the new state
     */
    @Override
    public void setHangarState(final String state) {
        model.setHangarState(state);
        hangarState.set(state);
    }

    /**
     * Update distance in both model and view property.
     * Must be called from JavaFX Application Thread.
     * 
     * @param dist the new distance
     */
    @Override
    public void setDistance(final String dist) {
        try {
            model.setDistance(Float.parseFloat(dist));
        } catch (final NumberFormatException e) {
            model.setDistance(-1);
        }
        distance.set(dist);
    }

    /**
     * Update connection status in both model and view property.
     * Must be called from JavaFX Application Thread.
     * 
     * @param status the new status
     */
    @Override
    public void setConnectionStatus(final String status) {
        model.setConnectionState(status);
        connectionStatus.set(status);
    }

    /**
     * Get JavaFX property for binding to UI.
     * 
     * @return drone state property
     */
    public ReadOnlyStringProperty droneStateProperty() {
        return droneState.getReadOnlyProperty();
    }

    /**
     * Get JavaFX property for binding to UI.
     * 
     * @return hangar state property
     */
    public ReadOnlyStringProperty hangarStateProperty() {
        return hangarState.getReadOnlyProperty();
    }

    /**
     * Get JavaFX property for binding to UI.
     * 
     * @return distance property
     */
    public ReadOnlyStringProperty distanceProperty() {
        return distance.getReadOnlyProperty();
    }

    /**
     * Get JavaFX property for binding to UI.
     * 
     * @return connection status property
     */
    public ReadOnlyStringProperty connectionStatusProperty() {
        return connectionStatus.getReadOnlyProperty();
    }

    /**
     * Return a defensive/unmodifiable copy of available commands from the model.
     *
     * @return unmodifiable list of available commands
     */
    public java.util.List<it.unibo.dronehangar.remote.api.Command> getAvailableCommands() {
        return java.util.Collections.unmodifiableList(model.getAvailableCommands());
    }
}
