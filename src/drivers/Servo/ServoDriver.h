#include <configs.h>

struct SERVO
{
    uint8_t pin;
};

void setAngle(SERVO servo, double angle)
{
    angle = constrain(angle, 0, 180);
    uint16_t microSec = map(angle, 0, 180, 500, 2500); // Map angle to pulse width in microseconds

    pwmDriver.writeMicroseconds(servo.pin, microSec);
}