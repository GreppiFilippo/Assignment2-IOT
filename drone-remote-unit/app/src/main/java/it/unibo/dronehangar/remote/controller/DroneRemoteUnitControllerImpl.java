package it.unibo.dronehangar.remote.controller;

import java.util.Locale;
import java.util.Objects;

import it.unibo.dronehangar.remote.api.DroneRemoteUnitController;
import edu.umd.cs.findbugs.annotations.SuppressFBWarnings;
import it.unibo.dronehangar.remote.service.DroneConnectionService;
import it.unibo.dronehangar.remote.viewmodel.DroneRemoteUnitViewModel;
import javafx.application.Platform;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Label;
import javafx.scene.layout.TilePane;

/**
 * Controller of the Drone Remote Unit view.
 */
public final class DroneRemoteUnitControllerImpl implements DroneRemoteUnitController {

    @SuppressFBWarnings(value = "UWF_FIELD_NOT_INITIALIZED_IN_CONSTRUCTOR", justification = "set before initialize")
    private DroneRemoteUnitViewModel viewModel;
    private DroneConnectionService connectionService;

    @FXML
    private ComboBox<String> serialComboBox;
    @FXML
    private ComboBox<String> baudComboBox;
    @FXML
    private Button btnRefreshPorts;
    @FXML
    private TilePane buttonBox;
    @FXML
    private Label lblDroneState;
    @FXML
    private Label lblHangarState;
    @FXML
    private Label lblConnectionStatus;
    @FXML
    private Label lblDistance;
    @FXML
    private Label lblErrorMessage;

    @Override
    @SuppressFBWarnings(value = "EI_EXPOSE_REP2", justification = "Keeps ViewModel reference for binding")
    public void setViewModel(final DroneRemoteUnitViewModel viewModel) {
        this.viewModel = viewModel;
        this.connectionService = new DroneConnectionService(viewModel);
        this.connectionService.setErrorCallback(this::showError);
    }

    @FXML
    @Override
    public void initialize() {
        Objects.requireNonNull(viewModel, "ViewModel not initialized");
        /*
         * Bind labels to ViewModel properties
         */
        lblDroneState.textProperty().bind(viewModel.droneStateProperty());
        lblHangarState.textProperty().bind(viewModel.hangarStateProperty());
        lblDistance.textProperty().bind(viewModel.distanceProperty());
        lblConnectionStatus.textProperty().bind(viewModel.connectionStatusProperty());
        /*
         * Create command buttons dynamically
         */
        viewModel.getAvailableCommands().forEach(cmd -> {
            final Button btn = new Button(cmd.getName().toUpperCase(Locale.ROOT));
            btn.setPrefWidth(buttonBox.getPrefTileWidth());
            btn.setPrefHeight(buttonBox.getPrefTileHeight());
            btn.setOnAction(ev -> {
                if (!connectionService.isConnected()) {
                    showError("Not connected. Select a serial port first.");
                    return;
                }
                connectionService.sendCommand(cmd.getName());
                clearError();
            });
            buttonBox.getChildren().add(btn);
        });
        /*
         * Setup serial port and baud rate comboboxes
         */
        serialComboBox.setEditable(true);
        updateSerialPorts();
        serialComboBox.setOnAction(ev -> {
            final String port = serialComboBox.getValue();
            if (port != null) {
                connectionService.connect(port);
            }
        });
        btnRefreshPorts.setOnAction(ev -> updateSerialPorts());
        /*
         * Setup baud rate combobox
         */
        baudComboBox.getItems().addAll(connectionService.getSupportedBaudRates());
        if (!baudComboBox.getItems().isEmpty()) {
            baudComboBox.setValue(baudComboBox.getItems().get(0));
        }
        baudComboBox.setOnAction(ev -> {
            final String rate = baudComboBox.getValue();
            if (rate != null) {
                connectionService.setBaudRate(Integer.parseInt(rate));
            }
        });
        /*
         * Clear error message
         */
        lblErrorMessage.setVisible(false);
    }

    /**
     * Update the list of available serial ports in the combobox.
     */
    private void updateSerialPorts() {
        serialComboBox.getItems().clear();
        serialComboBox.getItems().addAll(connectionService.getAvailableCommPorts());
    }

    /**
     * Show an error message on the UI.
     * 
     * @param msg the error message to show
     */
    private void showError(final String msg) {
        Platform.runLater(() -> {
            lblErrorMessage.setText(msg);
            lblErrorMessage.setVisible(true);
        });
    }

    /**
     * Clear the error message from the UI.
     */
    private void clearError() {
        Platform.runLater(() -> {
            lblErrorMessage.setText("");
            lblErrorMessage.setVisible(false);
        });
    }

    @Override
    public void shutdown() {
        if (connectionService != null) {
            connectionService.shutdown();
        }
    }
}
