/*
 #include "Motor.h"
 PwmOut p1(p25);
 PwmOut p2(p24);
 PwmOut p3(p23);

 //#define motorLib_print_debugger
 #ifdef motorLib_print_debugger
 Serial pc1(p28, p27);
 #endif
 Motor::Motor()    //コンストラクタ
 {
 LPC_GPIO2 ->FIODIR |= MOTOR1 | MOTOR2 | MOTOR3;
 LPC_GPIO2 ->FIOCLR = MOTOR1 | MOTOR2 | MOTOR3;
 p1.period_ms(1);
 p2.period_ms(1);
 p3.period_ms(1);
 duty1 = duty2 = duty3 = 0;
 dir1 = dir2 = dir3 = 0;
 }

 void Motor::drive(int ch) {
 switch (ch) {
 case 1:
 if (dir1 == 2) {
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

 if (dir2 == 2)  //front
 {
 LPC_GPIO2 ->FIOCLR = MOTOR2;
 p2.write(duty2 / 1000.0);
 } else if (dir2 == 1)  //back
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
 duty1 = duty2 = duty3 = 0;
 dir1 = dir2 = dir3 = 0;
 drive(1);
 drive(2);
 drive(3);
 drive(4);
 }
 */
#include "Motor.h"

PwmOut p1(p25);
PwmOut p2(p24);
PwmOut p3(p23);

Motor::Motor()    //コンストラクタ
{

}

void Motor::init() {
	LPC_GPIO2 ->FIODIR |= MOTOR1 | MOTOR2 | MOTOR3;
	LPC_GPIO2 ->FIOCLR = MOTOR1 | MOTOR2 | MOTOR3;
	p1.period_ms(1);
	p2.period_ms(1);
	p3.period_ms(1);
	npwm1 = npwm2 = npwm3 = 0;
	pwm_sa = 0;
	duty1 = duty2 = duty3 = 0;
	oduty1 = oduty2 = oduty3 = 0;
	dir1 = dir2 = dir3 = 0;
	odir1 = odir2 = odir3 = 0;
	opwm1 = opwm2 = opwm3 = 0;
}

void Motor::drive(int ch) {
	switch (ch) {
	case 1:
		if (dir1 == 2) {
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

		if (dir2 == 2)  //front
				{
			LPC_GPIO2 ->FIOCLR = MOTOR2;
			p2.write(duty2 / 1000.0);
		} else if (dir2 == 1)  //back
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

void Motor::drive_L(int ch) {
	int duty;

	switch (ch) {
	case 1:
		npwm1 = duty1;
		pwm_sa = opwm1 - npwm1;  //差を計算

		/*逆方向の差が設定値を超えたら実行 　　ローパスの比を変えておく*/
		if (pwm_sa > Diff) {
			duty = (oduty1 * C + duty1 * D) / E;
			oduty1 = duty1 = duty;
			dir1 = odir1;
			if (duty1 == 0) {
				odir1 = 0;
				opwm1 = 0;
				pwm_sa = 0;
			}
		} else {
			duty = (oduty1 * A + duty1 * B) / M;
			oduty1 = duty1 = duty;
			odir1 = dir1;
			opwm1 = duty1;
		}
		drive(1);  //ドライブGo
		break;

	case 2:
		npwm2 = duty2;
		pwm_sa = opwm2 - npwm2;  //差を計算

		/*逆方向の差が設定値を超えたら実行 　　ローパスの比を変えておく*/
		if (pwm_sa > Diff) {
			duty = (oduty2 * C + duty2 * D) / E;
			oduty2 = duty2 = duty;
			dir2 = odir2;
			if (duty2 == 0) {
				odir2 = 0;
				opwm2 = 0;
				pwm_sa = 0;
			}
		} else {
			duty = (oduty2 * A + duty2 * B) / M;
			oduty2 = duty2 = duty;
			odir2 = dir2;
			opwm2 = duty2;
		}
		drive(2);
		break;
	case 3:
		npwm3 = duty3;
		pwm_sa = opwm3 - npwm3;  //差を計算

		/*逆方向の差が設定値を超えたら実行 　　ローパスの比を変えておく*/
		if (pwm_sa > Diff) {
			duty = (oduty3 * C + duty3 * D) / E;
			oduty3 = duty3 = duty;
			dir3 = odir3;
			if (duty3 == 0) {
				odir3 = 0;
				opwm3 = 0;
				pwm_sa = 0;
			}
		} else {
			duty = (oduty3 * A + duty3 * B) / M;
			oduty3 = duty3 = duty;
			odir3 = dir3;
			opwm3 = duty3;
		}
		drive(3);
		break;
	}
}
void Motor::off() {
	duty1 = duty2 = duty3 = 0;
	dir1 = dir2 = dir3 = 0;
	drive(1);
	drive(2);
	drive(3);
}
