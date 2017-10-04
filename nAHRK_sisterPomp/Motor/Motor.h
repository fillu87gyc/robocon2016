#ifndef MOTOR_INCLUDED
#define MOTOR_INCLUDED

#include "mbed.h"

//gpio2
#define MOTOR1 (0x1 << 11)

class Motor
{
public:
    Motor();
    void init();
    void drive(int ch);
    int duty1;
    int dir1;
};

#endif
