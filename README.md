# EasyFSM - Lightweight Finite State Machine for Embedded Projects

EasyFSM is a lightweight, easy-to-use finite state machine (FSM) library tailored for Arduino and other embedded platforms. It allows you to define state transitions, actions, and conditions in a clean and modular way, making your logic easier to manage, extend, and debug.

## Features
- Simple `State` structure with conditions and actions
- Supports sequential and any-match transition modes
- Timeout support per state
- Optional entry and exit callbacks

## Installation
1. Download or clone this repository.
2. Copy the `EasyFSM` folder to your Arduino `libraries/` directory.
3. Restart the Arduino IDE.

## Basic Usage
```cpp
#include <EasyFSM.h>

bool conditionA() { return digitalRead(2); }
void actionA() { digitalWrite(13, HIGH); }

bool conditionB() { return digitalRead(3); }
void actionB() { digitalWrite(13, LOW); }

State states[] = {
  { conditionA, actionA },
  { conditionB, actionB }
};

FSM fsm(states, 2);

void setup() {
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(13, OUTPUT);
  fsm.setMode(FSM_MODE_SEQUENTIAL);
}

void loop() {
  fsm.update();
}
```

## Advanced Features
- `onEnter` and `onExit` actions per state
- `timeoutMs` and `nextState` for timed transitions

## When to Use EasyFSM Instead of Boolean Logic

For simple control tasks, Boolean logic can be enough. For example:

```cpp
// Simple latch logic
bool r1 = !pb2State * (pb1State + r1);
```

This works great when:
- You only need 1–2 steps
- Conditions are simple and don’t rely on history
- You don’t require timeouts, delays, or error handling
- You don't need to track state transitions

However, **as systems grow**, logic becomes harder to manage, debug, and scale. That’s where **EasyFSM** becomes the better tool.

### Use FSM when:
- **You have multi-step sequences** that must follow a specific order.
- **Each step has its own condition** that needs to be met before transitioning.
- **Time-based actions** or **timeouts** are required (e.g. safety checks, retries).
- **States need entry/exit actions**, such as turning relays on/off.
- You want to **handle failures gracefully** with error states.
- **State transitions depend on inputs**, but only after certain events.

### Example:

```cpp
// Boolean logic gets messy here
bool step1 = cond1 && !step2 && !step3;
bool step2 = cond2 && step1 && !step3;
bool step3 = cond3 && step2;
```

This becomes unreadable and hard to debug as complexity increases.

### FSM makes this easier:

```cpp
State states[] = {
  { cond1, action1 },
  { cond2, action2 },
  { cond3, action3 }
};

FSM fsm(states, 3);
fsm.setMode(FSM_MODE_SEQUENTIAL);
```

EasyFSM separates conditions and actions, automatically remembers the current step, and handles transitions clearly and cleanly.

> **Rule of thumb:**  
> _If your logic starts needing “step” flags, timers, and nested ifs — it's time for FSM._

## License
MIT

