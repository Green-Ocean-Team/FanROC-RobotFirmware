#include <configs.h>

struct MOTOR
{
    uint8_t forward_pin;
    uint8_t backward_pin;
};

void setSpeed(MOTOR motor, double speed)
{
    speed = constrain(speed, -1, 1);
    uint16_t _speed = (uint16_t)(abs(speed) * 4095);

    if (speed > 0)
    {
        pwmDriver.setPin(motor.forward_pin, _speed);
        pwmDriver.setPin(motor.backward_pin, 0);
        
    }
    else if (speed < 0)
    {
        pwmDriver.setPin(motor.forward_pin, 0);
        pwmDriver.setPin(motor.backward_pin, _speed);
    } else {
        pwmDriver.setPin(motor.forward_pin, 0);
        pwmDriver.setPin(motor.backward_pin, 0);
    }
}

void stop(MOTOR motor)
{
    pwmDriver.setPin(motor.forward_pin, 0);
    pwmDriver.setPin(motor.backward_pin, 0);
}