#include <Adafruit_PWMServoDriver.h>
#include <PS2X_lib.h>
#include <Wire.h>

Adafruit_PWMServoDriver pwmDriver = Adafruit_PWMServoDriver();
PS2X ps2x;

struct SERVO
{
    uint8_t pin;
};



// Define servo pins
#define SERVO_0 2
#define SERVO_1 3
#define SERVO_2 4
#define SERVO_3 5
#define SERVO_4 6
#define SERVO_5 7

// Define motor channels
#define MOTOR_R1_CH1 8
#define MOTOR_R1_CH2 9

#define MOTOR_R2_CH1 10
#define MOTOR_R2_CH2 11

#define MOTOR_R3_CH1 12
#define MOTOR_R3_CH2 13

#define MOTOR_R4_CH1 14
#define MOTOR_R4_CH2 15

// Define PS2 controller pins
#define PS2_DAT 12 // MISO  19
#define PS2_CMD 13 // MOSI  23
#define PS2_SEL 15 // SS     5
#define PS2_CLK 14 // SLK   18

// Define speed 
#define MAX_SPEED 1.0 // Maximum speed
#define NORMAL_SPEED 0.8 // Minimum speed
#define MIN_SPEED 0.4 // Minimum speed for motors

const double JOYSTICK_DEADZONE = 0.1; // Deadzone threshold

void initRobot() {
    Wire.begin();
    pwmDriver.begin();
    pwmDriver.setPWMFreq(50); // Set frequency to 60 Hz
    pwmDriver.setOscillatorFrequency(27000000); // Set oscillator frequency
    Wire.setClock(400000); // Set I2C clock speed to 400k

    int err = -1;
    while (err != 0)
    {
        err = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, true, true);
    }
}

void setAngle(SERVO servo, double angle)
{
    angle = constrain(angle, 0, 180);
    uint16_t microSec = map(angle, 0, 180, 500, 2500); // Map angle to pulse width in microseconds

    pwmDriver.writeMicroseconds(servo.pin, microSec);
}