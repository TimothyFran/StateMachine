/**
 * @file AdvancedStateMachine.ino
 * @brief Advanced example with external control and logging
 * 
 * This example shows:
 * - Creating states with complex logic
 * - Communication between states
 * - Forcing state transitions
 * - Logging current state
 */

#include <StateMachine.h>

// Structure for data shared between states
struct SharedData {
    unsigned long executionStartTime;
    int cycleCount;
    bool errorFlag;
};

SharedData sharedData = {0, 0, false};

// State 1: Preparation
class PrepareState : public BaseState {
public:
    PrepareState() : BaseState(500, 4000, false) {}
    
    void boot() override {
        BaseState::boot();
        Serial.println("[PREPARE] System preparing...");
        sharedData.executionStartTime = millis();
    }
    
    StateExitCode handle() override {
        // Simulate preparation with progressive steps
        unsigned long elapsed = getElapsedTime();
        
        if (elapsed < 1000) {
            Serial.println("[PREPARE] Step 1: Hardware initialization");
        } else if (elapsed < 2000) {
            Serial.println("[PREPARE] Step 2: Loading configuration");
        } else if (elapsed < 3000) {
            Serial.println("[PREPARE] Step 3: Verifying connections");
            return StateExitCode::PROCEED_TO_NEXT;  // Preparation done
        }
        
        // Timeout handling
        if (isStateTimedOut()) {
            Serial.println("[PREPARE] TIMEOUT - Preparation failed");
            return StateExitCode::FAILED;
        }
        
        return StateExitCode::CONTINUE;
    }
    
    void close() override {
        Serial.println("[PREPARE] Preparation finished\n");
    }
    
    const char* getStateName() const override { return "PREPARE"; }
};

// State 2: Execution
class ExecuteState : public BaseState {
public:
    ExecuteState() : BaseState(500, 8000, false) {}
    
    void boot() override {
        BaseState::boot();
        Serial.println("[EXECUTE] Execution started...");
        sharedData.cycleCount++;
    }
    
    StateExitCode handle() override {
        unsigned long elapsed = getElapsedTime();
        
        // Simulate different execution phases
        if (elapsed % 2000 < 500) {
            Serial.print("*");
            if (elapsed % 2000 == 0) Serial.println();
        }
        
        // Check error flag
        if (sharedData.errorFlag) {
            Serial.println("[EXECUTE] Error detected!");
            sharedData.errorFlag = false;
            return StateExitCode::FAILED;
        }
        
        // Simulate completion after 6 seconds
        if (elapsed >= 6000) {
            Serial.println("\n[EXECUTE] Execution completed");
            return StateExitCode::PROCEED_TO_NEXT;
        }
        
        // Timeout handling
        if (isStateTimedOut()) {
            Serial.println("[EXECUTE] TIMEOUT");
            return StateExitCode::FAILED;
        }
        
        return StateExitCode::CONTINUE;
    }
    
    void close() override {
        Serial.println("[EXECUTE] Execution state finished\n");
    }
    
    const char* getStateName() const override { return "EXECUTE"; }
};

// State 3: Finalization
class FinalizeState : public BaseState {
public:
    FinalizeState() : BaseState(500, 3000, false) {}
    
    void boot() override {
        BaseState::boot();
        Serial.println("[FINALIZE] Finalization in progress...");
    }
    
    StateExitCode handle() override {
        unsigned long elapsed = getElapsedTime();
        
        if (elapsed < 1000) {
            Serial.println("[FINALIZE] Saving state");
        } else if (elapsed < 2000) {
            Serial.println("[FINALIZE] Closing connections");
        } else {
            Serial.println("[FINALIZE] Cycle completed successfully!");
            unsigned long totalTime = millis() - sharedData.executionStartTime;
            Serial.print("Total time: ");
            Serial.print(totalTime);
            Serial.println(" ms");
            
            return StateExitCode::PROCEED_TO_NEXT;  // Restart from first state
        }
        
        return StateExitCode::CONTINUE;
    }
    
    void close() override {
        Serial.println("[FINALIZE] Finalization finished\n");
    }
    
    const char* getStateName() const override { return "FINALIZE"; }
};

// Global instance
StateMachine stateMachine;

void setup() {
    Serial.begin(115200);
    delay(500);
    
    Serial.println("\n\n=== StateMachine Library - Advanced Example ===\n");
    
    // Register states
    std::vector<std::unique_ptr<BaseState>> states;
    states.push_back(std::make_unique<PrepareState>());
    states.push_back(std::make_unique<ExecuteState>());
    states.push_back(std::make_unique<FinalizeState>());
    
    stateMachine.registerStates(std::move(states));
    stateMachine.initialize();
    
    Serial.print("StateMachine started with ");
    Serial.print(stateMachine.getStateCount());
    Serial.println(" states\n");
}

void loop() {
    // Handle current state
    stateMachine.handleCurrentState();
    
    // Simulation: force errors via serial (type 'e' + Enter)
    if (Serial.available() > 0) {
        char cmd = Serial.read();
        if (cmd == 'e' || cmd == 'E') {
            sharedData.errorFlag = true;
            Serial.println(">> Error forced!");
        } else if (cmd == 'n' || cmd == 'N') {
            Serial.println(">> Forced transition to next state");
            stateMachine.forceNextStateTransition();
        } else if (cmd == 'r' || cmd == 'R') {
            Serial.println(">> Reset to first state");
            stateMachine.forceStateTransition(0);
        }
    }
    
    // Print current state every 10 seconds
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 10000) {
        lastPrint = millis();
        Serial.print("\n>> Current state: ");
        Serial.print(stateMachine.getCurrentStateName());
        Serial.print(" | Cycles completed: ");
        Serial.println(sharedData.cycleCount);
        Serial.println(">> Press 'e' for error, 'n' for next, 'r' for reset\n");
    }
}
