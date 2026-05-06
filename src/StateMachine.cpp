#include "StateMachine.h"

StateMachine::StateMachine() 
    : currentState(nullptr), currentStateIndex(0) {
}

void StateMachine::registerStates(std::vector<std::unique_ptr<BaseState>> states) {
    // Move states from input vector to internal vector
    registeredStates.clear();
    for (auto& state : states) {
        if (state) {
            registeredStates.push_back(std::move(state));
        }
    }
    
    currentStateIndex = 0;
}

void StateMachine::initialize() {
    // Verify that states have been registered
    if (registeredStates.empty()) {
        return;
    }
    
    // Start first registered state
    currentStateIndex = 0;
    currentState = std::move(registeredStates[currentStateIndex]);
    if (currentState) {
        currentState->boot();
    }
}

void StateMachine::handleCurrentState() {
    if (!currentState) {
        return;
    }
    
    // Execute current state's logic
    StateExitCode currentStateHandleResult = currentState->handle();

    switch (currentStateHandleResult) {

        case StateExitCode::ERROR:
            // Critical error - transition to next state
            transitionToNextState();
            break;

        case StateExitCode::TIMED_OUT:
            if (currentState->SHOULD_HALT_ON_TIMEOUT) {
                // Critical timeout - restart device
                ESP.restart();
            } else {
                // Non-critical timeout - transition to next state
                transitionToNextState();
            }
            break;

        case StateExitCode::PROCEED_TO_NEXT:
            // Normal transition to next state
            transitionToNextState();
            break;

        case StateExitCode::FAILED:
            // Non-critical failure - return to previous state
            if (currentStateIndex > 0) {
                transitionToState(currentStateIndex - 1);
            } else {
                // If in first state, restart from beginning
                transitionToState(0);
            }
            break;

        case StateExitCode::CONTINUE:
            // No action - state continues
            break;

        default:
            // Unknown exit code - ignore
            break;
    }

}

const char* StateMachine::getCurrentStateName() const {
    if (currentState) {
        return currentState->getStateName();
    }
    return "NO_STATE";
}

void StateMachine::forceNextStateTransition() {
    transitionToNextState();
}

void StateMachine::forceStateTransition(uint8_t index) {
    transitionToState(index);
}

void StateMachine::transitionToNextState() {
    if (!currentState) {
        return;
    }
    
    // Cleanup current state
    currentState->close();
    currentState.reset();
    
    // Check if there are more states to execute
    if (currentStateIndex < registeredStates.size()) {
        transitionToState(currentStateIndex + 1);
    } else {
        // Reached end - restart from first state
        transitionToState(0);
    }
}

void StateMachine::transitionToState(uint8_t index) {
    if (index >= registeredStates.size()) {
        return;
    }
    
    // Cleanup current state
    if (currentState) {
        currentState->close();
        currentState.reset();
    }
    
    // Transition to specified state
    currentStateIndex = index;
    currentState = std::move(registeredStates[currentStateIndex]);
    if (currentState) {
        currentState->boot();
    }
}
