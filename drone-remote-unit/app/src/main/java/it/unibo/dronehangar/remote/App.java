package it.unibo.dronehangar.remote;

import java.util.logging.Logger;

/**
 * Main class of the application.
 */
public final class App {

    private static final Logger LOG = Logger.getLogger(App.class.getName());

    private App() {
        // Prevent instantiation of main class
    }

    /**
     * A simple hello method to demonstrate logging.
     */
    public static void hello() {
        LOG.fine("Hello from App constructor");
    }

    /**
     * Main entry point of the application.
     * 
     * @param args the command line arguments
     */
    public static void main(final String... args) {
        hello();
    }
}
