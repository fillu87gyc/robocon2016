#include "Motor.h"
PwmOut p1(p26);
PwmOut p2(p25);
PwmOut p3(p24);
PwmOut p4(p23);

//#define motorLib_print_debugger
#ifdef motorLib_print_debugger
Serial pc1(p28, p27);
#endif
Motor::Motor()    //コンストラクタ
{

}

void Motor::init() {
	LPC_GPIO2 ->FIODIR |= MOTOR1 | MOTOR2 | MOTOR3 | MOTOR4;
	LPC_GPIO2 ->FIOCLR = MOTOR1 | MOTOR2 | MOTOR3 | MOTOR4;
	p1.period_us(83);
	p2.period_us(83);
	p3.period_us(83);
	p4.period_us(83);
	duty1 = duty2 = duty3 = duty4 = 0;
	dir1 = dir2 = dir3 = dir4 = 0;
}

void Motor::drive(int ch) {
#ifdef motorLib_print_debugger
	pc1.printf("%4d\t%4d\t%4d\t%4d\t \t%d %d %d %d\n\r", duty1, duty2, duty3, duty4, dir1, dir2, dir3, dir4);
#endif
	switch (ch) {
	case 1:

		if (dir1 == 2)    //front
				{
			LPC_GPIO2 ->FIOCLR = MOTOR1;
			p1.write(duty1 / 1000.0);
		} else if (dir1 == 1)    //back
				{
			LPC_GPIO2 ->FIOSET = MOTOR1;
			p1.write((1000 - duty1) / 1000.0);
		} else  //break
		{
			LPC_GPIO2 ->FIOSET = MOTOR1;
			p1.write(1.0f);
		}
		break;
	case 2:

		if (dir2 == 1)  //front
				{
			LPC_GPIO2 ->FIOCLR = MOTOR2;
			p2.write(duty2 / 1000.0);
		} else if (dir2 == 2)  //back
				{
			LPC_GPIO2 ->FIOSET = MOTOR2;
			p2.write((1000 - duty2) / 1000.0);
		} else  //brake
		{
			LPC_GPIO2 ->FIOSET = MOTOR2;
			p2.write(1.0f);
		}
		break;
	case 3:

		if (dir3 == 2)  //front
				{
			LPC_GPIO2 ->FIOCLR = MOTOR3;
			p3.write(duty3 / 1000.0);
		} else if (dir3 == 1)  //back
				{
			LPC_GPIO2 ->FIOSET = MOTOR3;
			p3.write((1000 - duty3) / 1000.0);
		} else  //brake
		{
			LPC_GPIO2 ->FIOSET = MOTOR3;
			p3.write(1.0f);
		}
		break;
	case 4:
		if (dir4 == 1)  //front
				{
			LPC_GPIO2 ->FIOCLR = MOTOR4;
			p4.write(duty4 / 1000.0);
		} else if (dir4 == 2)  //back
				{
			LPC_GPIO2 ->FIOSET = MOTOR4;
			p4.write((1000 - duty4) / 1000.0);
		} else  //brake
		{
			LPC_GPIO2 ->FIOSET = MOTOR4;
			p4.write(1.0f);
		}
		break;
	}
}

void Motor::off() {

	dir1 = dir2 = dir3 = dir4 = 0;
	duty1 = duty2 = duty3 = duty4 = 0;
	drive(1);
	drive(2);
	drive(3);
	drive(4);
}

void Motor::allDrive() {
	drive(1);
	drive(2);
	drive(3);
	drive(4);
}

