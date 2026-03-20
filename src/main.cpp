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
  // =================================================================
  // 1. NON-SYNC RELOAD SERVO CONTROL 
  // =================================================================
  if (reloading && (millis() - previousMillis >= reloadDelay))
  {
    reloading = false;          // Reset reloading flag
    setAngle(reloadServo, 175); // Move reload servo back to 175 degrees
  }

  // =================================================================
  // 2. READ PS2 CONTROLLER INPUT & CONTROL MOTORS (Runs every 30ms)
  // =================================================================
  if (millis() - lastPS2ReadMillis >= PS2_POLL_RATE)
  {
    lastPS2ReadMillis = millis();
    
    bool isConnected = ps2x.read_gamepad(false, 0); 
    double targetLy = 0.0;
    double targetRx = 0.0;
    
    if (!isConnected) 
    {
      // FAIL-SAFE: Mất kết nối -> Đưa mục tiêu về 0 để tự động phanh mềm
      targetLy = 0.0;
      targetRx = 0.0;
      stopIntake();
      intakeState = false; 
    }
    else
    {
      // NORMAL OPERATION: Có kết nối thì đọc dữ liệu
      targetLy = (ps2x.Analog(PSS_LY) - 128) / 127.0;
      targetRx = (ps2x.Analog(PSS_RX) - 128) / 127.0;

      targetLy = applyDeadzone(targetLy, JOYSTICK_DEADZONE);
      targetRx = applyDeadzone(targetRx, JOYSTICK_DEADZONE);

      // Cài đặt giới hạn tốc độ
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

      // Xử lý nút bấm Intake
      if (ps2x.ButtonPressed(PSB_L2))
      {
        intakeState = !intakeState;
        if (intakeState) intake();
        else stopIntake();
      }

      if (ps2x.Button(PSB_PAD_DOWN))
      {
        intakeReverse();
      }
      else if (ps2x.ButtonReleased(PSB_PAD_DOWN))
      {
        if(intakeState == false) stopIntake(); // Chỉ dừng nếu L2 không đang bật
      }

      // Xử lý Outake Servo
      if (ps2x.Button(PSB_R2)) { setAngle(outakeServo, 120); }
      else { setAngle(outakeServo, 85); }

      // Xử lý Reload Servo
      if (ps2x.ButtonPressed(PSB_BLUE))
      {
        if (!reloading)
        {
          reloading = true;
          setAngle(reloadServo, 90);
          previousMillis = millis();
        }
      }

      // Xử lý Bottom Servo L
      if(ps2x.ButtonPressed(PSB_GREEN))
      {
        isHandleOpen = !isHandleOpen;
        if (isHandleOpen) { setAngle(bottomServoL, 90); }
        else { setAngle(bottomServoL, 180); }
      }
    } 
    // =================================================================
    // SPEED RAMPING (Tính toán nội suy vận tốc mỗi 30ms)
    // =================================================================
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

    robotMove(currentRx, currentLy);
  }
}