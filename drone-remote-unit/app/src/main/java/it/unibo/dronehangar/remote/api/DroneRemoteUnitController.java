package it.unibo.dronehangar.remote.api;

import it.unibo.dronehangar.remote.viewmodel.DroneRemoteUnitViewModel;

/**
 * Controller interface for the Drone Remote Unit.
 */
public interface DroneRemoteUnitController {

    /**
     * Shutdown method to stop all threads gracefully.
     */
    void shutdown();

    /**
     * Sets the ViewModel for this controller.
     * 
     * @param viewModel the ViewModel to set
     */
    void setViewModel(DroneRemoteUnitViewModel viewModel);

    /**
     * Initializes the controller. This method is called after the FXML fields
     * have been injected.
     */
    void initialize();

}
