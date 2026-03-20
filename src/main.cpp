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

unsigned long lastPS2ReadMillis = 0;
const int PS2_POLL_RATE = 30; // Polling rate in milliseconds for reading PS2 controller input

bool isHandleOpen = false; // Flag to indicate if the handle is open
bool intakeState = false;  // Flag to track the state of the intake (on/off)

double currentLy = 0.0;
double currentRx = 0.0;
const double RAMP_SPEED = 0.05; // Adjust this value to control how quickly the robot ramps up to the target speed

SERVO outakeServo = {SERVO_5};
SERVO bottomServoL = {SERVO_4};
SERVO reloadServo = {SERVO_3};

void setup()
{
  Serial.begin(9600);
  initRobot();
  setAngle(outakeServo, 85);  // Initialize outake servo to 0 degrees
  setAngle(bottomServoL, 180); // Initialize bottom left servo to 90 degrees
  setAngle(reloadServo, 175); // Initialize bottom right servo to 90 degrees
}

void loop()
{

  if (millis() - lastPS2ReadMillis >= PS2_POLL_RATE)
  {
    lastPS2ReadMillis = millis();
    
    // Check if the controller is still connected
    bool isConnected = ps2x.read_gamepad(false, 0); 
    
    if (!isConnected) 
    {
      // FAIL-SAFE: Emergency stop if controller is disconnected
      robotMove(0, 0); 
      stopIntake();
      return; 
    }
  }
  ps2x.read_gamepad();


  /* OLD CONTROL CODE WITHOUT SPEED RAMPING:
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
  */

  double targetLy = (ps2x.Analog(PSS_LY) - 128) / 127.0;
  double targetRx = (ps2x.Analog(PSS_RX) - 128) / 127.0;

  targetLy = applyDeadzone(targetLy, JOYSTICK_DEADZONE);
  targetRx = applyDeadzone(targetRx, JOYSTICK_DEADZONE);

  // Áp dụng giới hạn tốc độ theo nút bấm cho targetLy và targetRx...
  if (ps2x.Button(PSB_L1))
  {
    targetRx = constrain(targetRx, -MIN_SPEED, MIN_SPEED);
    targetLy = constrain(targetLy, -MIN_SPEED, MIN_SPEED);
  }
  else if (ps2x.Button(PSB_R1))
  {
    targetRx = constrain(targetRx, -MAX_SPEED, MAX_SPEED);
    targetLy = constrain(targetLy, -MAX_SPEED, MAX_SPEED);
  }
  else
  {
    targetRx = constrain(targetRx, -NORMAL_SPEED, NORMAL_SPEED);
    targetLy = constrain(targetLy, -NORMAL_SPEED, NORMAL_SPEED);
  }

  // SPEED RAMPING:
  // INCREASE OR DECREASE currentLy TOWARDS targetLy BY RAMP_SPEED, AND DO THE SAME FOR currentRx
  if (currentLy < targetLy) {
    currentLy += RAMP_SPEED;
    if (currentLy > targetLy) currentLy = targetLy;
  } else if (currentLy > targetLy) {
    currentLy -= RAMP_SPEED;
    if (currentLy < targetLy) currentLy = targetLy;
  }

  if (currentRx < targetRx) {
    currentRx += RAMP_SPEED;
    if (currentRx > targetRx) currentRx = targetRx;
  } else if (currentRx > targetRx) {
    currentRx -= RAMP_SPEED;
    if (currentRx < targetRx) currentRx = targetRx;
  }

  // MOVE THE ROBOT USING currentLy AND currentRx INSTEAD OF targetLy AND targetRx
  robotMove(currentRx, currentLy);

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

  if(ps2x.ButtonPressed(PSB_GREEN))
  {
    isHandleOpen = !isHandleOpen; // Toggle handle state
    if (isHandleOpen)
    {
      setAngle(bottomServoL, 90); // Move bottom left servo to 0 degrees
    }
    else
    {
      setAngle(bottomServoL, 180); // Move bottom left servo back to 90 degrees
    }
  }
}