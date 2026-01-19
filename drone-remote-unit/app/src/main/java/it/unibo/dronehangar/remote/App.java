package it.unibo.dronehangar.remote;

import java.util.List;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import it.unibo.dronehangar.remote.api.Command;
import it.unibo.dronehangar.remote.controller.DroneRemoteUnitControllerImpl;
import it.unibo.dronehangar.remote.model.DroneRemoteUnitModelImpl;
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

    private final List<Command> commandList = List.of(
        () -> "OPEN",
        () -> "CLOSE",
        () -> "TAKEOFF",
        () -> "LAND"
    );

    /**
     * Main entry point of the application.
     *
     * @param args the commands line arguments
     */
    public static void main(final String... args) {
        LOGGER.info("Starting Drone Remote Unit application");
        launch(args);
    }

    @Override
    public void start(final Stage primaryStage) throws Exception {
        LOGGER.info("Initializing JavaFX application");
        final FXMLLoader loader = new FXMLLoader(getClass().getResource("/view/DroneRemoteUnit.fxml"));
        loader.setControllerFactory(param -> 
            new DroneRemoteUnitControllerImpl(
                new DroneRemoteUnitModelImpl(this.commandList)
            )
        );
        final Parent root = loader.load();
        final Scene scene = new Scene(root);

        primaryStage.setTitle("Drone Remote Unit");
        primaryStage.setScene(scene);
        primaryStage.show();
        LOGGER.info("Application window shown");
    }

    @Override
    public void stop() {
        LOGGER.info("Shutting down Drone Remote Unit application");
    }
}
