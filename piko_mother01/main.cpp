/*

 packet protocol
 --------+---------------+----------
 while   |   0xEB        | header
 1byte   |   joystick_LX | 0 ~ 100 (Median:50)
 2byte   |   joystick_LY | 0 ~ 100 (Median:50)
 3byte   |   joystick_RX | 0 ~ 100 (Median:50)
 4byte   |   joystick_RY | 0 ~ 100 (Median:50)
 5byte   |   button1     | bit
 6byte   |   button2     | bit
 7byte   |   Sum         | unchar
 8byte   |   0xAF        | ender
 --------+---------------+-----------

 button bit assign
 -+-----+-------+
 | bit0 | Select |
 | bit1 | L3     |
 | bit2 | R3     |
 | bit3 | Start  |
 | bit4 | Up     |
 | bit5 | Rigth  |
 | bit6 | Down   |
 | bit7 | Left   |
 -+-----+-------+--
 | bit0 | L2     |
 | bit1 | R2     |
 | bit2 | L1     |
 | bit3 | R1     |
 | bit4 | △    	 |
 | bit5 | Circle |
 | bit6 | Cross  |
 | bit7 | Square |
 -+------+-------+

 */

#include "mbed.h"
#include "myLIB/MySerial.h"
#include "myLIB/button.h"
#include "Motor/Motor.h"
//#include "myLIB/Servo.h"

typedef unsigned char unchar;

//distributes the number
const int Ly = 0, Ry = 1, Lx = 2, Rx = 3, sw1 = 4, sw2 = 5, Sum = 6, end = 7;

/*make instance*/

//Serial
#ifdef print_debugger
Serial pc(p28, p27);
#endif
Serial slave(p9, p10);
MySerial FEP01TJ(&slave);
DigitalOut led1(p15), led2(p16), led3(p17);
DigitalOut valve1(p26), valve2(p21);

Motor motor;
DigitalOut myservo(P2_11);
Ticker pseudo_pwmNeutral, pseudo_pwmUp;
Button dualshock;

int servo_begin;

//global
unchar re_data[8] = { 0 };
double joystick[2] = { 0.0, 0.0 };

unchar CheckSum(unchar *data) {
	return data[Lx] + data[Ly] + data[Rx] + data[Ry] + data[sw1] + data[sw2];
}

bool uartRead() {
	int count = 0;
	while (1) {
		if (FEP01TJ.input() == 0xEB)
			break;
		else {
			count++;
			if (count > 15)
				return false;
		}
	}
	re_data[Lx] = FEP01TJ.input();   //Lx 飛ばす
	re_data[Ly] = FEP01TJ.input();   //Ly
	re_data[Rx] = FEP01TJ.input();   //Rx 飛ばす
	re_data[Ry] = FEP01TJ.input();   //Ry
	re_data[sw1] = FEP01TJ.input();   //sw1
	re_data[sw2] = FEP01TJ.input();   //sw2
	re_data[Sum] = FEP01TJ.input();   //CheckSum
	re_data[end] = FEP01TJ.input();   //end
#ifdef print_debugger
#ifdef Serial_debugger
			pc.printf("%d %d %d %d 0x%X 0x%X 0x%X 0x%X sum = 0x%x\n\r",re_data[Lx],re_data[Ly],re_data[Rx],re_data[Ry],re_data[sw1],re_data[sw2],re_data[Sum],re_data[end],CheckSum(re_data));
#endif
#endif
	if ((re_data[Sum] == CheckSum(re_data)) && (re_data[end] == 0xAF))
		return true;
	else
		return false;

}

void input_val() {
	joystick[Ly] = 2.0 * ((re_data[Ly] / 100.0) - 0.5);
	joystick[Ry] = 2.0 * ((re_data[Ry] / 100.0) - 0.5);
	dualshock.set_Data((re_data[sw2] << 8) + re_data[sw1]);
}

double myAbs(double a) {
	if (a > 0.0)
		return a;
	else
		return (-1.0) * a;
}

void normal_move() {
	double velocity = 1.0;
	if (dualshock.push(Cross))
		velocity = 0.4;
	//前転後転の確認
	motor.dir1 = (joystick[Ly] > 0.0) + 1;
	motor.dir2 = (joystick[Ry] > 0.0) + 1;
	joystick[Ly] = myAbs(joystick[Ly]);
	joystick[Ry] = myAbs(joystick[Ry]);

	int motor_speed[2] = { (int) (joystick[Ly] * 999.9 * velocity),
			(int) (joystick[Ry] * 999.9 * velocity) };

	const int L = 0, R = 1;
	static bool switch_move = true;
	if (dualshock.rise(Square))
		switch_move = !(switch_move);

	if (switch_move) {
		motor.duty1 = motor_speed[L];	//L
		motor.duty2 = motor_speed[L];	//L
		motor.dir2 = motor.dir1;
	} else {
		motor.duty1 = motor_speed[L];	//個別制御
		motor.duty2 = motor_speed[R];
	}

#ifdef print_debugger
#ifdef check_joystick
	pc.printf("L%4d R%4d dir %d , %d\n\r",motor.duty1,motor.duty2,motor.dir1,motor.dir2);
#endif
#endif
	//足回り
	motor.drive(1);
	motor.drive(2);

}

void take_up() {

	if (dualshock.push(Up))
		motor.dir3 = 2;
//ウォームギアが噛んでるから逆転しない
	else if (dualshock.push(Down))
		motor.dir3 = 1;
	else
		motor.dir3 = 0;

	//motor.dir3 = dualshock.push(Up);
//減速する
	double speed_ctrl = 1.0;
	if (dualshock.push(Cross))
		speed_ctrl = 0.4;

	motor.duty3 = 900 * speed_ctrl;
	//led1.write(dualshock.push(Up));

	motor.drive(3);
}

void valve_ctrl() {
	if (dualshock.push(Start)){//&& dualshock.push(Select)) {
		valve1.write(1);
		valve2.write(1);
		led3.write(1);
		led2.write(0);
		servo_begin = 1;
		//led1.write(1);
	} else {
		valve1.write(0);
		valve2.write(0);
		led3.write(0);
	}
	led1.write(servo_begin);
}
void servo_neutral() {
	myservo.write(1);
	wait_us(1500);
	myservo.write(0);
	wait_us(8500);
	pseudo_pwmNeutral.attach_us(&servo_neutral, 10000);
}

void servo_up() {
	//while (!dualshock.rise(Circle)) {
	myservo.write(1);
	wait_us(1000);
	myservo.write(0);
	wait_us(9000);
	//}	//基本的にここで終了
	pseudo_pwmUp.attach_us(&servo_up, 10000);
}

void servo_ctrl() {
	if (servo_begin && dualshock.push(Triangle)) {
		pseudo_pwmNeutral.detach();
		servo_up();
	}
}

void setup() {
#ifdef print_debug
	pc.baud(9600);
	pc.printf("program begin\n\r");
#endif
	slave.baud(38400);
	motor.init();
	servo_neutral();
	servo_begin = 0;
}

void loop() {
	static int failed_counter = 0;
	if (uartRead()) {
		input_val();
		normal_move();
		valve_ctrl();
		servo_ctrl();
		take_up();
		led2.write(1);
		//wait_ms(20);
	} else {
		failed_counter++;
		if (failed_counter > 3) {
			//通信が復帰することなく3回失敗してしまった。
			//motor.off();
		}
		led2.write(0);
	}
}
int main() {
	setup();
	for (;;)
		loop();
}
