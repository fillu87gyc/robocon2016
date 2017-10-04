#ifndef MOTOR_INCLUDED
#define MOTOR_INCLUDED

#include "mbed.h"

//gpio2
#define MOTOR1 (0x1 << 4)
#define MOTOR2 (0x1 << 5)
#define MOTOR3 (0x1 << 6)
#define MOTOR4 (0x1 << 7)
class Motor
{
public:
	Motor();
	void init();
	void drive(int ch);
	void off();
	void allDrive();
	int duty1;
	int duty2;
	int duty3;
	int duty4;
	int dir1;
	int dir2;
	int dir3;
	int dir4;
};

#endif
