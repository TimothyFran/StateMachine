# StateMachine Library

A small, reusable Finite State Machine (FSM) library for Arduino, ESP32, and other embedded platforms.

## Features

- Lightweight and generic
- Automatic state transitions using exit codes
- Optional timeouts per state
- Recoverable fallback to the previous state
- Works with Arduino IDE and PlatformIO
- Cycle-driven: no threading, just main loop execution

## Installation

### Arduino IDE

1. Download the library ZIP from GitHub
2. Open **Sketch → Include Library → Add .ZIP Library**
3. Select the ZIP file

### PlatformIO

Add this to `platformio.ini`:

```ini
lib_deps =
    https://github.com/TimothyFran/StateMachine.git
```

Or copy the `StateMachine` folder into your project's `lib/` directory.

## Quick Overview

Use `StateMachine` to register a sequence of states, then call `initialize()` in `setup()` and `handleCurrentState()` in `loop()`.

Each state inherits from `BaseState` and implements:

- `boot()` — called when the state starts
- `handle()` — called repeatedly while the state is active
- `close()` — called when the state finishes
- `getStateName()` — optional debug name

States return a `StateExitCode` to decide what happens next.

## Example

```cpp
#include <StateMachine.h>

class MyState : public BaseState {
public:
    MyState() : BaseState(500, 5000, false) {}

    void boot() override {
        Serial.println("State started");
    }

    StateExitCode handle() override {
        if (someCondition) {
            return StateExitCode::PROCEED_TO_NEXT;
        }
        return StateExitCode::CONTINUE;
    }

    void close() override {
        Serial.println("State ended");
    }

    const char* getStateName() const override {
        return "MY_STATE";
    }
};

StateMachine stateMachine;

void setup() {
    std::vector<std::unique_ptr<BaseState>> states;
    states.push_back(std::make_unique<MyState>());
    states.push_back(std::make_unique<AnotherState>());

    stateMachine.registerStates(std::move(states));
    stateMachine.initialize();
}

void loop() {
    stateMachine.handleCurrentState();
}
```

## How It Works

1. Register your states
2. Initialize the machine in `setup()`
3. Call `handleCurrentState()` repeatedly in `loop()`
4. The library runs the current state, checks its exit code, and transitions as needed

## Notes

- Use `BaseState` constructors to set state timeout and check interval
- `StateExitCode::FAILED` can fallback to the previous state
- `StateExitCode::TIMED_OUT` handles state timeout conditions

## License

MIT
