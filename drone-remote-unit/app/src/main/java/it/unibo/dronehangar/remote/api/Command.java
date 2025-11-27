package it.unibo.dronehangar.remote.api;

/**
 * Command interface representing a command that can be sent or received.
 */
@FunctionalInterface
public interface Command {

    /**
     * Get the name of the command.
     * 
     * @return the name of the command
     */
    String getName();
}
