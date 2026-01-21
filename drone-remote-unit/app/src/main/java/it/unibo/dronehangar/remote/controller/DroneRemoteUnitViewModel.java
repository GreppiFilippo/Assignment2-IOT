package it.unibo.dronehangar.remote.controller;

import edu.umd.cs.findbugs.annotations.SuppressFBWarnings;
import it.unibo.dronehangar.remote.model.DroneRemoteUnitModelImpl;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;

/**
 * ViewModel that bridges between pure Model and JavaFX View.
 * Contains JavaFX properties for UI binding.
 */
@SuppressFBWarnings(value = "EI_EXPOSE_REP", justification = "JavaFX properties are meant to be exposed for binding")
public final class DroneRemoteUnitViewModel {

    private final DroneRemoteUnitModelImpl model;
    private final StringProperty droneState;
    private final StringProperty hangarState;
    private final StringProperty distance;
    private final StringProperty connectionStatus;

    /**
     * Constructor.
     * 
     * @param model the pure model
     */
    public DroneRemoteUnitViewModel(final DroneRemoteUnitModelImpl model) {
        this.model = model;
        this.droneState = new SimpleStringProperty(model.droneStateProperty());
        this.hangarState = new SimpleStringProperty(model.hangarStateProperty());
        this.distance = new SimpleStringProperty(model.distanceProperty());
        this.connectionStatus = new SimpleStringProperty(model.connectionStatusProperty());
    }

    /**
     * Update drone state in both model and view property.
     * Must be called from JavaFX Application Thread.
     * 
     * @param state the new state
     */
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
    public void setDistance(final String dist) {
        model.setDistance(dist);
        distance.set(dist);
    }

    /**
     * Update connection status in both model and view property.
     * Must be called from JavaFX Application Thread.
     * 
     * @param status the new status
     */
    public void setConnectionStatus(final String status) {
        model.setConnectionStatus(status);
        connectionStatus.set(status);
    }

    /**
     * Get JavaFX property for binding to UI.
     * 
     * @return drone state property
     */
    public StringProperty droneStateProperty() {
        return droneState;
    }

    /**
     * Get JavaFX property for binding to UI.
     * 
     * @return hangar state property
     */
    public StringProperty hangarStateProperty() {
        return hangarState;
    }

    /**
     * Get JavaFX property for binding to UI.
     * 
     * @return distance property
     */
    public StringProperty distanceProperty() {
        return distance;
    }

    /**
     * Get JavaFX property for binding to UI.
     * 
     * @return connection status property
     */
    public StringProperty connectionStatusProperty() {
        return connectionStatus;
    }

    /**
     * Get the underlying model.
     * 
     * @return the model
     */
    public DroneRemoteUnitModelImpl getModel() {
        return model;
    }
}
