#pragma once

#include <Arduino.h>

/**
 * @brief Enum that defines state exit codes
 * 
 * Each state returns one of these codes to determine how the StateMachine
 * should proceed to the next execution. The state's behavior depends on
 * which code is returned from the handle() method.
 */
enum class StateExitCode {
    /**
     * @brief State continues execution in next loop iteration
     * The state will remain active and handle() will be called again
     */
    CONTINUE = 0,
    
    /**
     * @brief Transition to the next state in the sequence
     * The current state's close() will be called, then next state's boot()
     */
    PROCEED_TO_NEXT = 1,
    
    /**
     * @brief State has reached its timeout
     * Behavior depends on SHOULD_HALT_ON_TIMEOUT:
     * - If true: device restarts via ESP.restart()
     * - If false: transitions to next state
     */
    TIMED_OUT = 2,
    
    /**
     * @brief Critical error occurred
     * Transitions to the next state. Use when recovery is not possible.
     */
    ERROR = 3,
    
    /**
     * @brief Non-critical failure occurred
     * Transitions back to the previous state. Use for recoverable errors.
     * If already in first state, transitions to first state again.
     */
    FAILED = 4
};
