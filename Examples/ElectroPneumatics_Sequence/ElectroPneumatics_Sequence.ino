#include <EasyFSM.h>

// ELECTROPNUEMATICS REQUIREMENT //
  /* System Overview:
    A pneumatic system automates a sequence involving two double-acting cylinders (Cylinder A and Cylinder B), each controlled by two solenoid valves:

      Y1 – Extend Cylinder A (A+)
      Y2 – Retract Cylinder A (A−)
      Y3 – Extend Cylinder B (B+)
      Y4 – Retract Cylinder B (B−)

    The system includes material sensors:

      S1 – Detects material at Cylinder A’s retracted position
      S2 – Detects material at Cylinder B’s retracted position

    The system is started and stopped via pushbuttons:

      PB1 – Start button
      PB2 – Stop button

    Each cylinder has limit switches or sensors indicating extension/retraction:

      RA_EXT, RA_RET, RB_EXT, RB_RET

    Control Sequence:
      1. Idle (Wait for Start)

        -The system waits for the operator to press PB1 (Start).
        -On start, the running flag is set to true.

      2. Material Check

        -Cylinder A will not extend if S1 does not detect material.
        -Ensures only when material is present, the sequence proceeds.
      
      3. Extend Cylinder A (A+)

        -Activates Y1 and waits for RA_EXT to confirm full extension.
        -If not extended within timeout, or S2 (material at B) is not detected, it proceeds to retract.

      4. Extend Cylinder B (B+)

        -Activates Y3 and waits for RB_EXT signal.

      5. Retract Cylinder A (A−)

        -Activates Y2 and waits for RA_RET signal.

      6. Retract Cylinder B (B−)

        -Activates Y4 and waits for RB_RET signal.

      7. Loop

        -After retraction of Cylinder B, the sequence returns to the Material Check step and repeats until PB2 is pressed.

      8. Stop (Emergency or Operator)

        -At any point, pressing PB2 sets the system to stop, deactivates all solenoids, and resets the FSM.

    Safety & Logic Features:
      -Material Presence Dependency: Ensures Cylinder A does not operate without a workpiece.
      -Timeout Logic: Avoids indefinite waiting if extension fails or material is not detected.
      -Modular FSM Design: Makes the sequence easy to read, maintain, and expand.

    */

// Pin definitions
#define Y1 2  // A+
#define Y2 3  // A-
#define Y3 4  // B+
#define Y4 5  // B-

#define S1 6  // Material sensor A
#define S2 7  // Material sensor B

#define PB_START 8
#define PB_STOP 9

#define TIMEOUT_A_EXTEND 3000  // 3 seconds timeout

bool running = false;
unsigned long aExtendStart = 0;

// --- Conditions ---
bool isStartPressed() { return digitalRead(PB_START) == LOW; }
bool isStopPressed()  { return digitalRead(PB_STOP) == LOW; }

bool materialDetectedA() { return digitalRead(S1) == HIGH; }
bool materialDetectedB() { return digitalRead(S2) == HIGH; }

bool extendATimedOut() {
  return millis() - aExtendStart >= TIMEOUT_A_EXTEND;
}

bool shouldStartLoop() { return isStartPressed(); }
bool shouldStopLoop() { return isStopPressed(); }

// FSM control flags
bool stepRunning = false;

// --- Actions ---
void extendA() {
  digitalWrite(Y1, HIGH);
  digitalWrite(Y2, LOW);
  aExtendStart = millis();
  Serial.println("Extending A");
}

void retractA() {
  digitalWrite(Y1, LOW);
  digitalWrite(Y2, HIGH);
  Serial.println("Retracting A");
}

void extendB() {
  digitalWrite(Y3, HIGH);
  digitalWrite(Y4, LOW);
  Serial.println("Extending B");
}

void retractB() {
  digitalWrite(Y3, LOW);
  digitalWrite(Y4, HIGH);
  Serial.println("Retracting B");
}

void stopAll() {
  digitalWrite(Y1, LOW);
  digitalWrite(Y2, LOW);
  digitalWrite(Y3, LOW);
  digitalWrite(Y4, LOW);
  Serial.println("Stopped All Outputs");
}

// --- Composed Conditions ---
bool conditionExtendA() {
  return running && materialDetectedA();
}

bool conditionRetractA() {
  return running && extendATimedOut() && !materialDetectedB();
}

bool conditionExtendB() {
  return running;
}

bool conditionRetractB() {
  return running;
}

// --- FSM States ---
State states[] = {
  { shouldStartLoop, nullptr, [](){ running = true; Serial.println("STARTED"); }, nullptr, 1 },

  { conditionExtendA, extendA, nullptr, nullptr, 2 },
  { conditionRetractA, retractA, nullptr, nullptr, 3 },
  { conditionExtendB, extendB, nullptr, nullptr, 4 },
  { conditionRetractB, retractB, nullptr, nullptr, 1 },

  { shouldStopLoop, stopAll, [](){ running = false; Serial.println("STOPPED"); }, nullptr, 0 }
};

FSM fsm(states, sizeof(states) / sizeof(State));

void setup() {
  Serial.begin(9600);

  pinMode(Y1, OUTPUT);
  pinMode(Y2, OUTPUT);
  pinMode(Y3, OUTPUT);
  pinMode(Y4, OUTPUT);

  pinMode(S1, INPUT);
  pinMode(S2, INPUT);

  pinMode(PB_START, INPUT_PULLUP);
  pinMode(PB_STOP, INPUT_PULLUP);

  fsm.setMode(FSM_MODE_SEQUENTIAL);
}

void loop() {
  fsm.update();
}
