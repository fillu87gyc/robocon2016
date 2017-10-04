#ifndef MOTOR_INCLUDED
#define MOTOR_INCLUDED

#include "mbed.h"

//gpio2
#define MOTOR1 (0x1 << 7)
#define MOTOR2 (0x1 << 8)
#define MOTOR3 (0x1 << 11)

class Motor {
public:
	Motor();
	void init();
	void drive(int ch);
	void off();
	void allDrive();
	void drive_LP(int ch);
	void allDrive_LP();
	//int duty_old;
	int duty1;
	int duty2;
	int duty3;
	int dir1;
	int dir2;
	int dir3;
private:
	int dir1_old;
	int dir2_old;
	int dir3_old;
	int duty1_old;
	int duty2_old;
	int duty3_old;

	int abs(int a) {
		if (a > 0)
			return a;
		else
			return -a;
	}

	static const int UP_MOTOR_SPEED = 50;	//50
	static const int DOWN_MOTOR_SPEED = 20;	//20
	static const int DIFF = 200;			//200
	static const int FORWARD = 1;
	static const int BACK = 2;
	static const int MAGNIFICATION = 3;		//0への収束速度を決める(倍率)
	static const int MOTOR_STOP_DUTY = 0;
};

#endif
