#include "mbed.h"
#include "Motor/Motor.h"
#include "myLIB/mySerial.h"
#include "myLIB/button.h"
DigitalOut led1(p15);
DigitalOut led2(p16);
DigitalOut led3(p17);

Serial pc(p28, p27);   //デバック用シリアル
Serial slave(p9, p10);
MySerial FEP01TJ(&slave);
typedef unsigned char unchar;
#define CAN_BUS
#ifdef CAN_BUS
CAN can(p30, p29);
CANMessage msg;
#endif
Button dualshock;
const int Ly = 0, Ry = 1, Lx = 2, Rx = 3, sw1 = 4, sw2 = 5, Sum = 6, end = 7;

Motor motor;  //Motorクラスのインスタンスを作成
int joyLx, joyLy, joyRx, joyRy;

Timeout L2t, R2t;
const float TUNE_180 = 0.86f;
bool L2state = false, R2state = false;
int failed_counter = 0;

unchar re_data[8] = { 0 };
//#define print_debugger

void stop_tune_L2() {
	L2state = false;
}
void stop_tune_R2() {
	R2state = false;
}

void motor_move() {
	led3.write(1);
//傾いている方向を確認する
	const double PI = 3.1416;
	double rad = atan2((double) (joyLy), (double) (joyLx));
	rad = (rad + PI / 8.0) * 4.0 / PI;
	if (rad < 0.0)
		rad += 8.0;

	int way = (int) rad;

	//初めのほうは斜めに動かさないようにする
		//if ((motor_speed < 600)) {
		switch (way) {
		case 1:
			way = 2;
			break;
		case 3:
			way = 2;
			break;
		case 5:
			way = 6;
			break;
		case 7:
			way = 6;
			break;
		default:
			break;
		}

	int ctrl_dir13[8] = { 1, 1, 1, 0, 2, 2, 2, 0 };
	int ctrl_dir02[8] = { 2, 0, 1, 1, 1, 0, 2, 2 };

	motor.dir1 = ctrl_dir02[way];
	motor.dir2 = ctrl_dir13[way];
	motor.dir3 = ctrl_dir02[way];
	motor.dir4 = ctrl_dir13[way];

//傾き具合を三平方の定理で確認
	int motor_speed = (int) (sqrt((double) (joyLx * joyLx + joyLy * joyLy))
			* 10.0);
	if (motor_speed > 1000)
		motor_speed = 999;

	double ctrl_speed = 0.6;
//×ボタンで減速する
	if ((re_data[sw2] & (0x01 << 6)) != 0)
		ctrl_speed = 0.4;

	motor_speed *= ctrl_speed;
	/*各チャンネルにpwm値を格納*/
	motor.duty1 = motor_speed;
	motor.duty2 = motor_speed;
	motor.duty3 = motor_speed;
	motor.duty4 = motor_speed;

	motor.allDrive();
}

/*右旋回*/
void machine_turn_right() {
	led3.write(0);
	/*各チャンネルにPWM値を格納*/

	double speed_ctrl = 0.9;
	if ((re_data[sw2] & (0x01 << 6)) != 0)
		speed_ctrl = 0.5;

	motor.duty1 = (int) (700.0 * speed_ctrl);
	motor.duty2 = (int) (700.0 * speed_ctrl);
	motor.duty3 = (int) (700.0 * speed_ctrl);
	motor.duty4 = (int) (700.0 * speed_ctrl);

	/*回転方向*/
	motor.dir1 = 2;
	motor.dir2 = 1;
	motor.dir3 = 1;
	motor.dir4 = 2;

	motor.allDrive();
}

/*左旋回*/
void machine_turn_left() {
	led3.write(0);

	double speed_ctrl = 0.9;
	if ((re_data[sw2] & (0x01 << 6)) != 0)
		speed_ctrl = 0.5;

	/*各チャンネルにPWM値を格納*/
	motor.duty1 = (int) (700.0 * speed_ctrl);
	motor.duty2 = (int) (700.0 * speed_ctrl);
	motor.duty3 = (int) (700.0 * speed_ctrl);
	motor.duty4 = (int) (700.0 * speed_ctrl);

	/*回転方向*/
	motor.dir1 = 1;
	motor.dir2 = 2;
	motor.dir3 = 2;
	motor.dir4 = 1;

	motor.allDrive();
}

void machine_turn_left_auto(int pwm){
	led3.write(0);
	motor.duty1 = pwm;
	motor.duty2 = pwm;
	motor.duty3 = pwm;
	motor.duty4 = pwm;
	motor.dir1 = 1;
	motor.dir2 = 2;
	motor.dir3 = 2;
	motor.dir4 = 1;

	motor.allDrive();
}
void machine_turn_right_auto(int pwm){
	led3.write(0);
	motor.duty1 = pwm;
	motor.duty2 = pwm;
	motor.duty3 = pwm;
	motor.duty4 = pwm;

	motor.dir1 = 2;
	motor.dir2 = 1;
	motor.dir3 = 1;
	motor.dir4 = 2;

	motor.allDrive();
}

void normal_move() {
	if (dualshock.push(Left)) {
		machine_turn_left();
	} else if (dualshock.push(Right)) {
		machine_turn_right();
	} else
		motor_move();
}
//足回りの動き
void machine_move() {
	if (R2state) {
		machine_turn_right_auto(700);
		return;
	} else if (L2state) {
		machine_turn_left_auto(700);
		return;
	} else if (dualshock.rise(Start)) {
		R2state = true;
		R2t.attach(&stop_tune_R2, TUNE_180);
		L2state = false;
		//machine_move();	 //もう一度
	} else if (dualshock.rise(Select)) {
		L2state = true;
		L2t.attach(&stop_tune_L2, TUNE_180);
		R2state = false;
		//machine_move();  //もう一度
	} else {
		normal_move();
		return;
	}
}
bool uratRead() {
	int count = 0;
	while (1) {
		if (FEP01TJ.input() == 0xEB)
			break;
		else {
			count++;
			if (count > 15) {
#ifdef print_debugger
				pc.printf("not found head\t%d\n\r",headData);
#endif
				return false;
			}
		}
	}
	re_data[Lx] = FEP01TJ.input();
	re_data[Ly] = FEP01TJ.input();
	re_data[Rx] = FEP01TJ.input();
	re_data[Ry] = FEP01TJ.input();
	re_data[sw1] = FEP01TJ.input();
	re_data[sw2] = FEP01TJ.input();
	re_data[Sum] = FEP01TJ.input();
	re_data[end] = FEP01TJ.input();

	unchar CheckSum = re_data[Lx] + re_data[Ly] + re_data[Rx] + re_data[Ry]
			+ re_data[sw1] + re_data[sw2];

	if (re_data[Sum] == CheckSum && re_data[end] == 0xAF) {
//Successful
		return true;
	} else {
//failed
#ifdef print_debugger
		pc.printf("%d,%d,%d,%d   0x%X 0x%X %d 0x%X\n\r", re_data[Lx], re_data[Ly],
				re_data[Rx], re_data[Ry], re_data[sw1], re_data[sw2], re_data[Sum],
				re_data[end]);
#endif
		return false;
	}
}
#ifdef CAN_BUS
void can_make(bool dang) {
	if (dang) {
//通信成功時
		for (int i = 0; i < 7; i++) {
			msg.data[i] = re_data[i];
		}
		msg.data[7] = 0x01;
	} else {
//失敗時コントローラをニュートラル
		msg.data[Lx] = msg.data[Ly] = msg.data[Rx] = msg.data[Ry] = 50;
		msg.data[sw1] = msg.data[sw2] = 0x00;
		msg.data[Sum] = msg.data[end] = 0x00;
	}
	can.write(msg);
}
#endif

void setUP() {
	led1.write(1);
#ifdef print_debugger
	pc.baud(9600);
	pc.printf("begin\n\r");
#endif
	slave.baud(38400);
#ifdef CAN_BUS
	msg.id = 17;
	can.frequency(500 * 1000);
#endif
	motor.init();
}

void loop() {
	if (uratRead()) {

//中心を0にする。
//-100 から +100までの範囲に設定
		joyLx = (re_data[Lx] - 50) * 2;
		joyLy = (re_data[Ly] - 50) * 2;
		joyRx = (re_data[Rx] - 50) * 2;
		joyRy = (re_data[Ry] - 50) * 2;
#ifdef CAN_BUS
		can_make(true);
#endif
		dualshock.set_data((re_data[sw2] << 8) + re_data[sw1]);
		led2.write(1);
//足まわりの動作
		machine_move();
//今までの失敗をなかったコトに
		failed_counter = 0;
		wait_ms(19);
	} else {
		failed_counter++;
		if (failed_counter > 3) {
			//通信が復帰することなく3回失敗してしまった。
#ifdef print_debugger
			pc.printf("stopMotor\n\r");
#endif
			motor.off();
#ifdef CAN_BUS
			can_make(false);
#endif
		}
		led2.write(0);
	}
}

int main() {
	setUP();
	for (;;)
		loop();
}
