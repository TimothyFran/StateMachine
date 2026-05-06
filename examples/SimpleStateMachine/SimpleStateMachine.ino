/**
 * @file SimpleStateMachine.ino
 * @brief Basic example of using the StateMachine library
 * 
 * This example shows how to create a simple state machine with three states.
 * InitState simulates initialization (3 seconds)
 * ProcessState simulates processing (5 seconds)
 * FinalState simulates finalization (2 seconds)
 */

#include <StateMachine.h>

// Declaration of custom states
class InitState : public BaseState {
public:
    InitState() : BaseState(500, 3000, false) {}  // Timeout 3 seconds, non-critical
    
    void boot() override {
        BaseState::boot();
        Serial.println("[INIT] Initialization started");
    }
    
    StateExitCode handle() override {
        if (BaseState::handle() != StateExitCode::CONTINUE) {
            return StateExitCode::TIMED_OUT;  // Timeout reached
        }
        
        // Simulate initialization
        if (getElapsedTime() % 1000 == 0) {
            Serial.print(".");
        }
        
        // If timeout not reached, proceed to next state
        return StateExitCode::CONTINUE;
    }
    
    void close() override {
        Serial.println("\n[INIT] Initialization completed");
    }
    
    const char* getStateName() const override {
        return "INIT";
    }
};

class ProcessState : public BaseState {
public:
    ProcessState() : BaseState(500, 5000, false) {}  // Timeout 5 seconds
    
    void boot() override {
        BaseState::boot();
        Serial.println("[PROCESS] Processing started");
    }
    
    StateExitCode handle() override {
        if (BaseState::handle() != StateExitCode::CONTINUE) {
            return StateExitCode::TIMED_OUT;  // Timeout reached
        }
        
        // Simulate processing
        unsigned long elapsed = getElapsedTime();
        if (elapsed % 1000 == 0) {
            Serial.print("*");
        }
        
        // If timeout not reached, proceed to next state
        return StateExitCode::CONTINUE;
    }
    
    void close() override {
        Serial.println("\n[PROCESS] Processing completed");
    }
    
    const char* getStateName() const override {
        return "PROCESS";
    }
};

class FinalState : public BaseState {
public:
    FinalState() : BaseState(500, 2000, false) {}  // Timeout 2 seconds
    
    void boot() override {
        BaseState::boot();
        Serial.println("[FINAL] Finalization started");
    }
    
    StateExitCode handle() override {
        if (BaseState::handle() != StateExitCode::CONTINUE) {
            return StateExitCode::PROCEED_TO_NEXT;  // Timeout reached - go to next
        }
        
        // Simulate finalization
        if (getElapsedTime() % 500 == 0) {
            Serial.print("-");
        }
        
        return StateExitCode::CONTINUE;
    }
    
    void close() override {
        Serial.println("\n[FINAL] Finalization completed - cycle restarted!");
    }
    
    const char* getStateName() const override {
        return "FINAL";
    }
};

// StateMachine instance
StateMachine stateMachine;

void setup() {
    Serial.begin(115200);
    delay(500);
    
    Serial.println("\n\n=== StateMachine Library - Simple Example ===\n");
    
    // Register states
    std::vector<std::unique_ptr<BaseState>> states;
    states.push_back(std::make_unique<InitState>());
    states.push_back(std::make_unique<ProcessState>());
    states.push_back(std::make_unique<FinalState>());
    
    stateMachine.registerStates(std::move(states));
    
    // Initialize state machine
    stateMachine.initialize();
    
    Serial.print("StateMachine initialized with ");
    Serial.print(stateMachine.getStateCount());
    Serial.println(" states");
}

void loop() {
    // Handle current state
    stateMachine.handleCurrentState();
    
    // Print current state name every 5 seconds
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 5000) {
        lastPrint = millis();
        Serial.print("\nCurrent state: ");
        Serial.print(stateMachine.getCurrentStateName());
        Serial.print(" (index: ");
        Serial.print(stateMachine.getCurrentStateIndex());
        Serial.println(")");
    }
}
