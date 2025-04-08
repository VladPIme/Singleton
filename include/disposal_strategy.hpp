#ifndef DISPOSAL_STRATEGY_HPP
#define DISPOSAL_STRATEGY_HPP

#include <cstdlib>  // for atexit
#include <stdexcept>

namespace pattern {

/**
 * Standard disposal strategy that schedules destruction at program exit
 * 
 * This strategy registers a destruction callback with atexit() and 
 * throws an exception if a dead instance is accessed.
 */
template <typename T>
class StandardDisposalStrategy {
public:
    /**
     * Register a function to be called at program termination
     * 
     * @param cleanup_func Function pointer to the cleanup function
     */
    static void RegisterForCleanup(void (*cleanup_func)()) {
        std::atexit(cleanup_func);
    }
    
    /**
     * Handle attempts to access a destroyed instance
     * 
     * This implementation throws an exception when a dead reference is detected.
     */
    static void HandleDeadInstance() {
        throw std::runtime_error("Attempting to access a destroyed singleton instance");
    }
};

/**
 * Immortal disposal strategy that never destroys the singleton
 * 
 * This strategy is useful for objects that should live for the entire
 * program duration and avoids potential issues with destruction order.
 */
template <typename T>
class ImmortalStrategy {
public:
    /**
     * Does not register for destruction
     */
    static void RegisterForCleanup(void (*)()) {
        // Intentionally empty - we don't want to destroy the instance
    }
    
    /**
     * No special handling needed as instance should never be destroyed
     */
    static void HandleDeadInstance() {
        // Should never be called
    }
};

/**
 * Resurrection strategy that allows recreation after destruction
 * 
 * This strategy registers for cleanup but silently allows recreation
 * of the instance if accessed after destruction.
 */
template <typename T>
class ResurrectionStrategy {
public:
    /**
     * Register for cleanup normally
     */
    static void RegisterForCleanup(void (*cleanup_func)()) {
        std::atexit(cleanup_func);
    }
    
    /**
     * Silently allow recreation of the instance
     */
    static void HandleDeadInstance() {
        // Intentionally empty - we allow recreation silently
    }
};

} // namespace pattern

#endif // DISPOSAL_STRATEGY_HPP