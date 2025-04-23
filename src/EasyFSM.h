#ifndef EASY_FSM_H
#define EASY_FSM_H

typedef bool (*ConditionFunc)();
typedef void (*ActionFunc)();

enum FSMMode {
  FSM_MODE_ANY,
  FSM_MODE_SEQUENTIAL
};

struct State {
  ConditionFunc condition;
  ActionFunc action;
  ActionFunc onEnter;
  ActionFunc onExit;
  int nextState;
  unsigned long timeoutMs;
  unsigned long enterTime;

  // Constructor for easier initialization
  State(ConditionFunc conditionFunc = nullptr,
        ActionFunc actionFunc = nullptr,
        ActionFunc onEnterFunc = nullptr,
        ActionFunc onExitFunc = nullptr,
        int nextState = -1,
        unsigned long timeoutMs = 0)
      : condition(conditionFunc),
        action(actionFunc),
        onEnter(onEnterFunc),
        onExit(onExitFunc),
        nextState(nextState),
        timeoutMs(timeoutMs),
        enterTime(0) {}
};


class FSM {
public:
  FSM(State* states, int numStates);

  void update();
  void setMode(FSMMode mode = FSM_MODE_ANY);
  int getCurrentStep() const;
  void setCurrentStep(int step);

private:
  State* states;
  int numStates;
  int currentStep;
  FSMMode mode;
};

#endif
