#pragma once

#include <Arduino.h>
#include "StateExitCode.h"

/**
 * @brief Abstract base class for all system states
 * 
 * Each state must implement boot(), handle() and close() methods
 * to manage initialization, execution, and cleanup respectively.
 * 
 * Default timing values for BaseState:
 * - STATUS_CHECK_INTERVAL: 1000ms
 * - STATE_TIMEOUT: 0 (no timeout)
 * - SHOULD_HALT_ON_TIMEOUT: false (no device restart on timeout)
 * 
 * Derived classes can have different values by calling the protected constructor
 * with specific parameters in their initialization list.
 */
class BaseState {
public:
    virtual ~BaseState() = default;

protected:
    /**
     * @brief Protected constructor with default timing values
     */
    BaseState() : STATUS_CHECK_INTERVAL(1000), STATE_TIMEOUT(0), SHOULD_HALT_ON_TIMEOUT(false) {}

    /**
     * @brief Protected constructor for derived classes with custom timing values
     * @param statusCheckInterval Interval for periodic status checks in ms
     * @param stateTimeout Maximum timeout for this state in ms (0 = no timeout)
     * @param shouldHaltOnTimeout If true, device restarts on timeout. Otherwise, transitions to next state.
     */
    explicit BaseState(unsigned long statusCheckInterval, unsigned long stateTimeout, bool shouldHaltOnTimeout)
        : STATUS_CHECK_INTERVAL(statusCheckInterval), STATE_TIMEOUT(stateTimeout), SHOULD_HALT_ON_TIMEOUT(shouldHaltOnTimeout) {}

public:

    /**
     * @brief Initialize the state when it becomes active
     * Called once when entering the state
     */
    virtual void boot() {
        bootedAt = millis();
    }

    /**
     * @brief Handle the main logic of the state
     * Called repeatedly in the main loop
     * @return StateExitCode that determines the next action
     */
    virtual StateExitCode handle() {
        if (isStateTimedOut()) {
            return StateExitCode::TIMED_OUT;
        }
        return StateExitCode::CONTINUE;
    }

    /**
     * @brief Cleanup the state before transitioning
     * Called once when exiting the state
     */
    virtual void close() = 0;

    /**
     * @brief Get the name of this state for debugging
     */
    virtual const char* getStateName() const = 0;

    /**
     * @brief Check if the state has timed out (if timeout is configured)
     * @return true if state has timed out and should be terminated
     */
    bool isStateTimedOut() const {
        if (STATE_TIMEOUT == 0) return false; // No timeout
        return (millis() - bootedAt) >= STATE_TIMEOUT;
    }

    /**
     * @brief Get elapsed time since state entry in milliseconds
     * @return Milliseconds elapsed
     */
    unsigned long getElapsedTime() const {
        return millis() - bootedAt;
    }

    /**
     * @brief Indicates whether to restart device on timeout
     */
    const bool SHOULD_HALT_ON_TIMEOUT;

protected:
    unsigned long lastStatusCheck = 0;
    unsigned long bootedAt = 0;

    // Read-only parameters set in constructor
    const unsigned long STATUS_CHECK_INTERVAL;
    const unsigned long STATE_TIMEOUT;
};
