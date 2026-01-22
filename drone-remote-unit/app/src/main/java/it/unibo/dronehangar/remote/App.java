package it.unibo.dronehangar.remote;

import java.io.IOException;
import java.util.List;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import it.unibo.dronehangar.remote.api.Command;
import it.unibo.dronehangar.remote.controller.DroneRemoteUnitControllerImpl;
import it.unibo.dronehangar.remote.model.DroneRemoteUnitModelImpl;
import it.unibo.dronehangar.remote.viewmodel.DroneRemoteUnitViewModel;
import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

/**
 * Main class of the application.
 */
public final class App extends Application {

    private static final Logger LOGGER = LoggerFactory.getLogger(App.class);
    private final List<Command> commandList = List.of(() -> "OPEN");

    @Override
    public void start(final Stage primaryStage) {
        LOGGER.debug("Initializing JavaFX application");

        try {
            final var model = new DroneRemoteUnitModelImpl(commandList);
            final var viewModel = new DroneRemoteUnitViewModel(model);
            final var controller = new DroneRemoteUnitControllerImpl();
            controller.setViewModel(viewModel);
            final FXMLLoader loader = new FXMLLoader(getClass().getResource("/view/DroneRemoteUnit.fxml"));
            loader.setControllerFactory(param -> controller);
            final Parent root = loader.load();
            final Scene scene = new Scene(root);
            primaryStage.setTitle("Drone Remote Unit");
            primaryStage.setScene(scene);
            primaryStage.show();
            LOGGER.debug("Application window shown");
        } catch (final IOException e) {
            LOGGER.error("Failed to initialize application", e);
        }
    }

    @Override
    public void stop() {
        LOGGER.info("Shutting down Drone Remote Unit application");
    }

    /**
     * Main method.
     * 
     * @param args command-line arguments
     */
    public static void main(final String... args) {
        LOGGER.info("::::::: Drone Remote Unit :::::::");
        launch(args);
    }
}
