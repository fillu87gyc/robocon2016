#include  "Motor.h"
#include "mbed.h"

PwmOut  pwm[] = {
	PwmOut(LED4),
	PwmOut(LED3),
	PwmOut(LED2),
	PwmOut(LED1)
};
DigitalOut  digital[] = {
	DigitalOut(LED4),
	DigitalOut(LED3),
	DigitalOut(LED2),
	DigitalOut(LED1)
};
/*
 Serial pc(p28, p27);
 */
Motor::Motor() {
	for (int i = 0; i < duty.length(); i++)
	{
		duty[i] = MOTOR_STOP_DUTY;
		dir[i] = 0;
		dir_old[i] = 0;
		duty_old[i] = 0;
	}
}

void Motor::init() {
	for (int i = 0; i < pwm.Length(); i++)
	{
		pwm[i] = 0.0f;
		pwm[i].period_ms(1);
		digital[i] = 0;
	}
}
void Motor::drive(int ch) {
	if (duty[ch] < MOTOR_STOP_DUTY) {
		//負だったら停止したいものだと考えておく
		duty[ch] = duty_old[ch] = MOTOR_STOP_DUTY;
	}
	if (dir[ch] == 1)
	{
		//front
		digital[ch] = 0;
		pwm[ch].write(duty[ch] / 1000.0);
	}
	else if (dir[ch] == 2)
	{
		//back
		digital[ch] = 1;
		p1.write((1000 - duty[ch]) / 1000.0);
	}
	else
	{
		//break
		digital[ch] = 1;
		pwm[ch].write(1.0f);
	}
}
void Motor::off() {
	for (int i = 0; i < duty.length(); i++)
	{
		dir[i] = FORWARD;
		duty[i] = MOTOR_STOP_DUTY;
		dir_old = FORWARD;
		duty_old = MOTOR_STOP_DUTY;
		drive(i);
	}
}

void Motor::allDrive() {

	drive(1);
	drive(2);
}
void Motor::allDrive_LP() {

	drive_LP(1);
	drive_LP(2);
}

void Motor::drive_LP(int ch)
{
	if (duty[ch] == MOTOR_STOP_DUTY)
	{
		//モータを止めようとしていた時の処理
		dir[ch] = dir_old[ch];
		duty[ch] = duty_old[ch] = (duty_old[ch] - DOWN_MOTOR_SPEED * MAGNIFICATION);
		drive(ch);
		return;
	}
	if (dir[ch] == dir_old[ch]) 
	{
		//回転方向が変わらないとき
		if (duty[ch] > duty_old[ch])
		{
			//加速したい
			duty[ch] = duty_old[ch] = (duty_old[ch] + UP_MOTOR_SPEED);
			drive(ch);
			return;
		}
		else
		{
			//減速
			duty[ch] = duty_old[ch] = (duty_old[ch] - DOWN_MOTOR_SPEED);
			drive(ch);
			return;
		}
	}
	else
	{
		//回転方向を切り替えたい時反応よく変化させるようMAGNIFICATIONを設定していい感じにする
		duty[ch] = duty_old[ch] = duty_old[ch] - DOWN_MOTOR_SPEED * MAGNIFICATION;
		if (duty[ch] < DIFF)
		{
			//もう殆ど止まっているとかんがえられる場合
			dir_old[ch] = dir[ch];
			duty_old[ch] = duty[ch];
			drive(ch);
			return;
		}
		else
		{
			//まだ十分速度が残っている場合
			dir[ch] = dir_old[ch];
			drive(ch);
			return;
		}
	}
}
