#include <Arduino.h>
#include <components/DriveTrain.h>

double applyDeadzone(double value, double deadzone) {
  if (fabs(value) < deadzone) return 0.0;
  return value;
}

bool top_limit = false; // Top limit switch state
bool bottom_limit = false; // Bottom limit switch state

unsigned long lastReadTime = 0;
const unsigned long readInterval = 10; // 10 ms

SERVO outakeServo = {SERVO_0};
SERVO bottomServo = {SERVO_1};

void setup() {
  Serial.begin(9600);
  initRobot();
  pinMode(25, INPUT_PULLDOWN); // Top limit switch
  pinMode(32, INPUT_PULLDOWN); // Lift down button

  setAngle(outakeServo, 90); // Initialize outake servo to 0 degrees
  setAngle(bottomServo, 90); // Initialize outake servo to 0 degrees
}

void loop() {
  unsigned long currentMillis = millis();
  ps2x.read_gamepad();

  double ly = (ps2x.Analog(PSS_LY) - 128) / 127.0;
  double rx = (ps2x.Analog(PSS_RX) - 128) / 127.0;

  ly = applyDeadzone(ly, JOYSTICK_DEADZONE);
  rx = applyDeadzone(rx, JOYSTICK_DEADZONE);

  if (ps2x.Button(PSB_L1))
  {
    rx = constrain(rx, -MIN_SPEED, MIN_SPEED);
    ly = constrain(ly, -MIN_SPEED, MIN_SPEED);
  } else if (ps2x.Button(PSB_R1))
  {
    rx = constrain(rx, -MAX_SPEED, MAX_SPEED);
    ly = constrain(ly, -MAX_SPEED, MAX_SPEED);
  } else
  {
    rx = constrain(rx, -NORMAL_SPEED, NORMAL_SPEED);
    ly = constrain(ly, -NORMAL_SPEED, NORMAL_SPEED);
  }

  robotMove(rx, ly);

  if (ps2x.Button(PSB_GREEN)) {
    setAngle(outakeServo, 18); // Set outake servo to 0 degrees
  } else if (ps2x.Button(PSB_BLUE)) {
    setAngle(outakeServo, 90); // Set outake servo to 90 degrees
  }

  if (ps2x.Button(PSB_PINK)) {
    setAngle(bottomServo, 180); // Set outake servo to 0 degrees
  } else if (ps2x.Button(PSB_RED)) {
    setAngle(bottomServo, 90); // Set outake servo to 90 degrees
  }
    

  if (currentMillis - lastReadTime >= readInterval) {
    lastReadTime = currentMillis;
    top_limit = digitalRead(25); // Read top limit switch
    bottom_limit = digitalRead(32); // Read bottom limit switch
  }

  if (ps2x.Button(PSB_PAD_UP) && !top_limit)
  {
    robotLiftUp();
  } else if (ps2x.Button(PSB_PAD_DOWN) && !bottom_limit) {
    robotLiftDown();
  } else liftStop();
}