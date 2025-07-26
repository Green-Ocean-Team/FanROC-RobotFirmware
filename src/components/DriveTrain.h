#include <drivers/Motor/MotorDriver.h>
/*
Move forward: M1 green, M2 red
Move backward: M1 red, M2 green
Turn left: M1 red, M2 red
Turn right: M1 green, M2 green

LIFT up: M3 red, M4 green
LIFT down: M3 green, M4 red
*/

MOTOR MOTOR_1 = {MOTOR_R1_CH1, MOTOR_R1_CH2};
MOTOR MOTOR_2 = {MOTOR_R2_CH1, MOTOR_R2_CH2};
MOTOR MOTOR_3 = {MOTOR_R3_CH1, MOTOR_R3_CH2};
MOTOR MOTOR_4 = {MOTOR_R4_CH1, MOTOR_R4_CH2};

void robotMove(double x, double y) {
    setSpeed(MOTOR_1, y + x);
    setSpeed(MOTOR_2, -(y - x));
}

void robotLiftUp() {
    setSpeed(MOTOR_3, -0.8);
    setSpeed(MOTOR_4, 0.8);
}

void robotLiftDown() {
    setSpeed(MOTOR_3, 0.8);
    setSpeed(MOTOR_4, -0.8);
}

void liftStop() {
    stop(MOTOR_3);
    stop(MOTOR_4);
}