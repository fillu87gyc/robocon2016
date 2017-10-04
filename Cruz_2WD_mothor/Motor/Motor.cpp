#include  "Motor.h"
#include "mbed.h"

PwmOut p1(p24);
PwmOut p2(p23);
PwmOut p3(p22);
/*
 Serial pc(p28, p27);
 */
Motor::Motor() {
	duty1 = duty2 = duty3 = MOTOR_STOP_DUTY;
	dir1 = dir2 = dir3 = FREAD;
	dir1_old = dir2_old = dir3_old = FREAD;
	duty1_old = duty2_old = duty3_old = MOTOR_STOP_DUTY;
}

void Motor::init() {
	LPC_GPIO2 ->FIODIR |= MOTOR1 | MOTOR2 | MOTOR3;
	LPC_GPIO2 ->FIOCLR = MOTOR1 | MOTOR2 | MOTOR3;
	p1.period_ms(1);
	p2.period_ms(1);
	p3.period_ms(1);
}
void Motor::drive(int ch) {
	switch (ch) {
	case 1:
		if (duty1 < MOTOR_STOP_DUTY) {
			duty1 = duty1_old = MOTOR_STOP_DUTY;
		}
		if (dir1 == 1)    //front
				{
			LPC_GPIO2 ->FIOCLR = MOTOR1;
			p1.write(duty1 / 1000.0);
		} else if (dir1 == 2)    //back
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
		if (duty2 < MOTOR_STOP_DUTY) {
			duty2 = duty2_old = MOTOR_STOP_DUTY;
		}
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
	}
}
void Motor::off() {
	//val init
	dir1 = dir2 = dir3 = FORWARD;
	duty1 = duty2 = duty3 = MOTOR_STOP_DUTY;
	dir1_old = dir2_old = dir3_old = FORWARD;
	duty1_old = duty2_old = duty3_old = MOTOR_STOP_DUTY;
	drive(1);
	drive(2);
}

void Motor::allDrive() {
#ifdef motorLib_print_debugger
	pc1.printf("%4d\t%4d\t%4d\t%4d\r%4d\n\r", duty1, duty2, duty3, duty4, duty_old);
#endif
	drive(1);
	drive(2);
}
void Motor::allDrive_LP() {
#ifdef motorLib_print_debugger
	pc1.printf("%4d\t%4d\t%4d\t%4d\r%4d\n\r", duty1, duty2, duty3, duty4, duty_old);
#endif
	drive_LP(1);
	drive_LP(2);
}

void Motor::drive_LP(int ch) {
	if (ch == 1) {
		if (duty1 == MOTOR_STOP_DUTY) {
			dir1 = dir1_old;
			duty1 = duty1_old = (duty1_old - DOWN_MOTOR_SPEED * MAGNIFICATION);
			drive(1);
			return;
		}
		if (dir1 == dir1_old) {
			if (duty1 > duty1_old) {
				//加速したい
				duty1 = duty1_old = (duty1_old + UP_MOTOR_SPEED);
				drive(1);
				return;
			} else {
				//減速
				duty1 = duty1_old = (duty1_old - DOWN_MOTOR_SPEED);
				drive(1);
				return;
			}
		} else {
#ifdef print_debugger
			pc.printf("dir no much \n\r");
#endif
			duty1 = duty1_old = duty1_old - DOWN_MOTOR_SPEED * MAGNIFICATION;
			if (duty1 < DIFF) {
				dir1_old = dir1;
				duty1_old = duty1;
				drive(1);
				return;
			} else {
				dir1 = dir1_old;
				drive(1);
				return;
			}
		}
	} else if (ch == 2) {
		if (duty2 == MOTOR_STOP_DUTY) {
			dir2 = dir2_old;
			duty2 = duty2_old = (duty2_old - DOWN_MOTOR_SPEED * MAGNIFICATION);
			drive(2);
			return;
		}
		if (dir2 == dir2_old) {
			if (duty2 > duty2_old) {
				//加速したい
				duty2 = duty2_old = (duty2_old + UP_MOTOR_SPEED);
				drive(2);
				return;
			} else {
				//減速
				duty2 = duty2_old = (duty2_old - DOWN_MOTOR_SPEED);
				drive(2);
				return;
			}
		} else {
#ifdef print_debugger
			pc.printf("dir no much \n\r");
#endif
			duty2 = duty2_old = duty2_old - DOWN_MOTOR_SPEED * MAGNIFICATION;
			if (duty2 < DIFF) {
				dir2_old = dir2;
				duty2_old = duty2;
				drive(2);
				return;
			} else {
				dir2 = dir2_old;
				drive(2);
				return;
			}
		}
	}
}
