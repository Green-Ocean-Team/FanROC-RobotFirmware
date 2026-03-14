#include <Arduino.h>
#include <components/DriveTrain.h>

double applyDeadzone(double value, double deadzone)
{
  if (fabs(value) < deadzone)
    return 0.0;
  return value;
}

unsigned long previousMillis = 0;      // Store the last time the reload servo was activated
const unsigned long reloadDelay = 450; // Delay in milliseconds for the reload servo
bool reloading = false;                // Flag to indicate if the reload servo is currently active

bool intakeState = false;  // Top limit switch state
bool bottom_limit = false; // Bottom limit switch state
bool endGameState = false; // End game state

SERVO outakeServo = {SERVO_5};
SERVO bottomServoL = {SERVO_4};
SERVO reloadServo = {SERVO_3};

void setup()
{
  Serial.begin(9600);
  initRobot();
  setAngle(outakeServo, 85);  // Initialize outake servo to 0 degrees
  setAngle(bottomServoL, 90); // Initialize bottom left servo to 90 degrees
  setAngle(reloadServo, 175); // Initialize bottom right servo to 90 degrees
}

void loop()
{
  ps2x.read_gamepad();

  double ly = (ps2x.Analog(PSS_LY) - 128) / 127.0;
  double rx = (ps2x.Analog(PSS_RX) - 128) / 127.0;

  ly = applyDeadzone(ly, JOYSTICK_DEADZONE);
  rx = applyDeadzone(rx, JOYSTICK_DEADZONE);

  if (ps2x.Button(PSB_L1))
  {
    rx = constrain(rx, -MIN_SPEED, MIN_SPEED);
    ly = constrain(ly, -MIN_SPEED, MIN_SPEED);
  }
  else if (ps2x.Button(PSB_R1))
  {
    rx = constrain(rx, -MAX_SPEED, MAX_SPEED);
    ly = constrain(ly, -MAX_SPEED, MAX_SPEED);
  }
  else
  {
    rx = constrain(rx, -NORMAL_SPEED, NORMAL_SPEED);
    ly = constrain(ly, -NORMAL_SPEED, NORMAL_SPEED);
  }

  robotMove(rx, ly);

  if (ps2x.ButtonPressed(PSB_L2))
  {
    intakeState = !intakeState; // Toggle intake state
    if (intakeState)
    {
      intake();
    }
    else
    {
      stopIntake();
    }
  }

  if (ps2x.Button(PSB_PAD_DOWN))
  {
    intakeReverse();
  }
  else if (ps2x.ButtonReleased(PSB_PAD_DOWN))
  {
    stopIntake();
  }

  if (ps2x.Button(PSB_R2))
  {
    setAngle(outakeServo, 120); // Move outake servo to 90 degrees
  }
  else
  {
    setAngle(outakeServo, 85); // Move outake servo back to 90 degrees
  }

  if (ps2x.ButtonPressed(PSB_BLUE))
  {
    if (!reloading)
    {
      reloading = true;          // Set reloading flag
      setAngle(reloadServo, 90); // Move reload servo to 90 degrees
      previousMillis = millis(); // Store the current time
    }
  }

  if (reloading && millis() - previousMillis >= reloadDelay)
  {
    reloading = false;          // Reset reloading flag
    setAngle(reloadServo, 175); // Move reload servo back to 175 degrees
  }
}