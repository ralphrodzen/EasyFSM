#include <Servo.h>
#include <EasyFSM.h>

// --- Servo Setup ---
Servo positionServo;
Servo gripperServo;

const int POSITION_SERVO_PIN = 9;
const int GRIPPER_SERVO_PIN = 10;

// --- Angles ---
const int HOME_ANGLE = 0;
const int PICKUP_ANGLE = 45;
const int PLACE_ANGLE = 90;

const int GRIPPER_OPEN = 0;
const int GRIPPER_CLOSE = 180;

// --- Tracking state ---
int currentPosition = HOME_ANGLE;
int currentGrip = GRIPPER_OPEN;

unsigned long actionStart = 0;
const unsigned long ACTION_DELAY = 1000;  // Simulated movement delay

bool wait(unsigned long delayTime) {
  return millis() - actionStart > delayTime;
}

// --- Conditions ---
bool atPickupPosition() { return currentPosition == PICKUP_ANGLE && wait(ACTION_DELAY); }
bool atPlacePosition() { return currentPosition == PLACE_ANGLE && wait(ACTION_DELAY); }
bool atHomePosition() { return currentPosition == HOME_ANGLE && wait(ACTION_DELAY); }
bool gripDone() { return wait(ACTION_DELAY); }
bool releaseDone() { return wait(ACTION_DELAY); }

// --- Actions ---
void moveToPickup() {
  positionServo.write(PICKUP_ANGLE);
  currentPosition = PICKUP_ANGLE;
  actionStart = millis();
}

void gripObject() {
  gripperServo.write(GRIPPER_CLOSE);
  currentGrip = GRIPPER_CLOSE;
  actionStart = millis();
}

void moveToPlace() {
  positionServo.write(PLACE_ANGLE);
  currentPosition = PLACE_ANGLE;
  actionStart = millis();
}

void releaseObject() {
  gripperServo.write(GRIPPER_OPEN);
  currentGrip = GRIPPER_OPEN;
  actionStart = millis();
}

void returnHome() {
  positionServo.write(HOME_ANGLE);
  currentPosition = HOME_ANGLE;
  actionStart = millis();
}

// --- States ---
State armStates[] = {
  { atPickupPosition, moveToPickup },
  { gripDone, gripObject },
  { atPlacePosition, moveToPlace },
  { releaseDone, releaseObject },
  { atHomePosition, returnHome }
};

FSM fsm(armStates, sizeof(armStates) / sizeof(State));

void setup() {
  Serial.begin(9600);
  positionServo.attach(POSITION_SERVO_PIN);
  gripperServo.attach(GRIPPER_SERVO_PIN);

  fsm.setMode(FSM_MODE_SEQUENTIAL);
  actionStart = millis(); // Start delay timer
}

void loop() {
  fsm.update();
}
