package it.unibo.dronehangar.remote.controller;

import java.util.Locale;

import it.unibo.dronehangar.remote.api.CommChannel;
import it.unibo.dronehangar.remote.api.DroneRemoteUnitController;
import it.unibo.dronehangar.remote.api.DroneRemoteUnitModel;
import it.unibo.dronehangar.remote.comm.JSSCCommChannel;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Label;
import javafx.scene.layout.TilePane;

/**
 * Controller for the drone remote unit.
 */
public final class DroneRemoteUnitControllerImpl implements DroneRemoteUnitController {

    private final CommChannel channel;
    private final DroneRemoteUnitModel model;

    @FXML
    private ComboBox<String> serialComboBox;
    @FXML
    private ComboBox<String> baudComboBox;
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

    /**
     * Constructor for DroneRemoteUnitControllerImpl.
     *
     * @param model the drone remote unit model to use
     */
    public DroneRemoteUnitControllerImpl(final DroneRemoteUnitModel model) {
        this.channel = new JSSCCommChannel();
        this.model = model;
    }

    @FXML
    // @SuppressWarnings("javafx:useFXML") // TODO: Fix false positive
    private void initialize() {
        /*
         * Baud Rate Selection
        */
        for (final var baudRate : this.channel.getSupportedBaudRates()) {
            this.baudComboBox.getItems().add(baudRate);
        }
        this.baudComboBox.setOnAction(event -> {
            final String selectedBaud = this.baudComboBox.getValue();
            if (selectedBaud != null) {
                this.channel.setBaudRate(Integer.parseInt(selectedBaud));
            }
        });
        /*
         * Command Buttons
        */
        for (final var cmd : this.model.getAvailableCommands()) {
            final var btn = new Button(cmd.getName().toUpperCase(Locale.ROOT));
            btn.setOnAction(event -> {
                channel.sendMsg(cmd.getName().toUpperCase(Locale.ROOT));
            });
            final double tileW = this.buttonBox.getPrefTileWidth();
            final double tileH = this.buttonBox.getPrefTileHeight();
            if (tileW > 0) {
                btn.setPrefWidth(tileW);
            }
            if (tileH > 0) {
                btn.setPrefHeight(tileH);
            }
            this.buttonBox.getChildren().add(btn);
        }
        /*
         * Serial Port Selection
        */
        this.serialComboBox.setOnAction(event -> {
            updateSerialPorts();
            final String selectedPort = this.serialComboBox.getValue();
            if (selectedPort != null) {
                this.channel.setCommPort(selectedPort);
            }
        });
        this.updateSerialPorts();
        /*
         * Label
        */
        // lblDroneState.textProperty().bind(model.droneStateProperty());
        // lblHangarState.textProperty().bind(model.droneStateProperty());
        // lblConnectionStatus.textProperty().bind( );
        // lblDistance.textProperty().bind(model.distanceProperty());
    }

    private void updateSerialPorts() {
        this.serialComboBox.getItems().clear();
        this.serialComboBox.getItems().addAll(
            this.channel.getAvailableCommPorts()
        );
    }
}
