#include "Motor.h"
PwmOut p1(p22);
Motor::Motor()    //コンストラクタ
{}

void Motor::init() {
	LPC_GPIO2 ->FIODIR |= MOTOR1;
	LPC_GPIO2 ->FIOCLR = MOTOR1;
	p1.period_ms(1);
	duty1 = dir1 = 0;
}

void Motor::drive(int ch) {
	switch (ch) {
	case 1:

		if (dir1 == 2){
			 //front
			LPC_GPIO2 ->FIOCLR = MOTOR1;
			p1.write(duty1/1000.0);
		} else if (dir1 == 1) {
		    //back
			LPC_GPIO2 ->FIOSET = MOTOR1;
			p1.write((1000-duty1)/1000.0);
		} else {
			//brake (free)
			LPC_GPIO2 ->FIOSET = MOTOR1;
			p1.write(1.0);
		}
		break;
	}
}



