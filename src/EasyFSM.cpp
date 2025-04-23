#include <Arduino.h>
#include "EasyFSM.h"

FSM::FSM(State* states, int numStates) {
  this->states = states;
  this->numStates = numStates;
  this->currentStep = 0;
  this->mode = FSM_MODE_ANY;
}

void FSM::setMode(FSMMode mode) {
  this->mode = mode;
}

int FSM::getCurrentStep() const {
  return currentStep;
}

void FSM::setCurrentStep(int step) {
  if (step >= 0 && step < numStates) {
    currentStep = step;
  }
}

void FSM::update() {
  if (mode == FSM_MODE_ANY) {
    for (int i = 0; i < numStates; i++) {
      if (states[i].condition()) {
        if (i != currentStep && states[currentStep].onExit) {
          states[currentStep].onExit();
        }

        currentStep = i;

        states[currentStep].action();

        if (states[currentStep].onEnter) {
          states[currentStep].onEnter();
        }

        // Set the enterTime when entering a new state
        states[currentStep].enterTime = millis();
        
        // Check for state timeout
        if (states[currentStep].timeoutMs > 0 &&
            millis() - states[currentStep].enterTime >= states[currentStep].timeoutMs) {
          // Timeout has occurred, transition to the next state
          if (states[currentStep].nextState != -1) {
            currentStep = states[currentStep].nextState;
          } else {
            currentStep = (currentStep + 1) % numStates;  // Loop to next state
          }
        }

        break;
      }
    }
  } else if (mode == FSM_MODE_SEQUENTIAL) {
    if (states[currentStep].condition()) {
      if (states[currentStep].onExit) {
        states[currentStep].onExit();
      }

      states[currentStep].action();

      if (states[currentStep].onEnter) {
        states[currentStep].onEnter();
      }

      if (states[currentStep].timeoutMs > 0 &&
          millis() - states[currentStep].enterTime >= states[currentStep].timeoutMs) {
        // Timeout has occurred, move to next state or defined nextState
        if (states[currentStep].nextState != -1) {
          currentStep = states[currentStep].nextState;
        } else {
          currentStep = (currentStep + 1) % numStates;  // Loop to next state
        }
      }
    }
  }
}
