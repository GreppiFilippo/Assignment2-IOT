#ifndef __COMMAND_QUEUE__
#define __COMMAND_QUEUE__

#include <Arduino.h>

#include "kernel/CommandType.hpp"

#ifndef CMD_QUEUE_SIZE
#define CMD_QUEUE_SIZE 10
#endif

/**
 * @brief Class representing a queue of commands with expiration handling.
 *
 */
class CommandQueue
{
   private:
    /**
     * @brief Structure representing a queued command with its timestamp.
     *
     */
    struct QueuedCommand
    {
        /// @brief The command type
        CommandType cmd;
        /// @brief The timestamp when the command was enqueued
        uint32_t timestamp;
    };

    QueuedCommand queue[CMD_QUEUE_SIZE];
    volatile int head;
    volatile int tail;
    volatile int count;

   public:
    /**
     * @brief Construct a new Command Queue object
     *
     */
    CommandQueue();

    /**
     * @brief Enqueue a command into the queue
     *
     * @param cmd The command to enqueue
     * @param now Current timestamp
     * @return true if the command was enqueued successfully
     * @return false if the queue is full
     */
    bool enqueue(CommandType cmd, uint32_t now);

    /**
     * @brief Consume a command from the queue
     *
     * @param cmd The command to consume
     * @return true if the command was found and consumed
     * @return false if the command was not found
     */
    bool consume(CommandType cmd);

    /**
     * @brief Remove expired commands from the queue
     *
     * @param now Current timestamp
     */
    void cleanupExpired(uint32_t now);

    /**
     * @brief Get the current size of the command queue
     *
     * @return int current size
     */
    int size() const { return count; }

    /**
     * @brief Get the capacity of the command queue
     *
     * @return int capacity
     */
    int capacity() const { return CMD_QUEUE_SIZE; }

    /**
     * @brief Check if the command queue is full
     *
     * @return true if full
     * @return false if not full
     */
    bool isFull() const { return count >= CMD_QUEUE_SIZE; }
    /**
     * @brief Check if the command queue is empty
     *
     * @return true if empty
     * @return false if not empty
     */
    bool isEmpty() const { return count == 0; }
};

#endif /* __COMMAND_QUEUE__ */
