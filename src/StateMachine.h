#pragma once

#include <Arduino.h>
#include <memory>
#include <vector>
#include "BaseState.h"

/**
 * @brief Manages a sequence of states with automatic transitions
 * 
 * StateMachine is responsible for:
 * - Registering states to be executed in sequence
 * - Initializing the first state
 * - Executing the current state's logic in the main loop
 * - Managing transitions between states based on exit codes
 * 
 * Supports:
 * - Transition to next state
 * - Return to previous state (on failure)
 * - Optional timeout for each state
 * - Detailed logging for debugging
 * 
 * @example
 * @code
 * StateMachine stateMachine;
 * 
 * std::vector<std::unique_ptr<BaseState>> states;
 * states.push_back(std::make_unique<StateA>());
 * states.push_back(std::make_unique<StateB>());
 * 
 * stateMachine.registerStates(std::move(states));
 * stateMachine.initialize();
 * 
 * // In main loop
 * void loop() {
 *     stateMachine.handleCurrentState();
 * }
 * @endcode
 */
class StateMachine {

public:
    /**
     * @brief Constructor for StateMachine
     */
    explicit StateMachine();

    /**
     * @brief Register an array of states to be executed in sequence
     * @param states Vector of unique pointers to states
     */
    void registerStates(std::vector<std::unique_ptr<BaseState>> states);

    /**
     * @brief Initialize the state machine and transition to first state
     * Must be called before handleCurrentState
     */
    void initialize();

    /**
     * @brief Handle the current state in main loop
     * This function must be called repeatedly in the program's main loop
     */
    void handleCurrentState();

    /**
     * @brief Get the name of the current state for debugging
     * @return Name of current state or "NO_STATE" if not initialized
     */
    const char* getCurrentStateName() const;

    /**
     * @brief Get the index of the current state
     * @return Current state index (0-based)
     */
    uint8_t getCurrentStateIndex() const { return currentStateIndex; }

    /**
     * @brief Get total number of registered states
     * @return Number of states
     */
    uint8_t getStateCount() const { return registeredStates.size(); }

    /**
     * @brief Force transition to the next state
     * Useful for external control of transitions
     */
    void forceNextStateTransition();

    /**
     * @brief Force transition to a specific state
     * @param index Index of desired state
     */
    void forceStateTransition(uint8_t index);

private:
    std::unique_ptr<BaseState> currentState;
    std::vector<std::unique_ptr<BaseState>> registeredStates;
    uint8_t currentStateIndex;
    
    /**
     * @brief Transition to the next state in array
     */
    void transitionToNextState();

    /**
     * @brief Transition to a specific state in array
     * @param index Index of state to transition to
     */
    void transitionToState(uint8_t index);

};
