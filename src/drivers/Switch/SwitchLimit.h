#include <configs.h>
#include <Arduino.h>

struct SWITCH_LIMIT
{
    uint8_t pin;
};

bool getSwitchState(SWITCH_LIMIT sw)
{
    return digitalRead(sw.pin);
}