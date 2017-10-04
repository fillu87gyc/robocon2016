/*

 packet protocol
 --------+---------------+----------
 while   |   0xEB        | header
 1byte	|   joystick_LX | 0 ~ 100 (Median:50)
 2byte   |   joystick_LY | 0 ~ 100 (Median:50)
 3byte        |	joystick_RX | 0 ~ 100 (Median:50)
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
 | bit4 | △		|
 | bit5 | Circle |
 | bit6 | Cross  |
 | bit7 | Square |
 -+------+-------+

 */

#include "mbed.h"
#include "myLIB/MySerial.h"
#include "myLIB/button.h"
#include "Motor/Motor.h"

//#define print_debugger
//#define check_joystick
//#define pomp_check
//#define CAN_BUS
//#define auto_ctrl

typedef unsigned char unchar;

//Define the number
const int Ly = 0, Ry = 1, Lx = 2, Rx = 3, sw1 = 4, sw2 = 5, Sum = 6, end = 7;
const int distanceL = 0, distanceR = 1, encoderL = 2, encoderR = 3;
//make instance
//Serial
Serial slave(p9, p10);
#ifdef print_debugger
Serial pc(p28, p27);
#endif
MySerial FEP01TJ(&slave);
//debug
DigitalOut led1(p15), led2(p16), led3(p17);
//valve attach
Timeout valve_stop1, valve_stop2, valve_stop3;
//limit senecer
DigitalIn senser_up(p19);
DigitalIn senser_low(p20);
#ifdef CAN_BUS
CAN can(p30,p29);
CANMessage msg;
#endif

//port 5 + port4
DigitalOut valve1(p26), valve2(p21), valve3(p25);
//port 1 + 2 + 3
Motor motor;

//port SPI
DigitalOut pomp1(p5), pomp2(p6), pomp3(p7), pomp4(P0_22);
Button dualshock;

//global
unchar re_data[8] = { 0 };
double joystick[4] = { 0.0, 0.0, 0.0, 0.0 };
bool pompState1 = false;
bool pompState2 = false;
bool pompState3 = false;
bool pompState4 = false;
bool arm_state_UP = false;
bool arm_state_DOWN = false;

const float OPEN_TIME = 0.3f; //300ms

inline unchar CheckSum(unchar *data) {
	return data[Lx] + data[Ly] + data[Rx] + data[Ry] + data[sw1] + data[sw2];
}

void stop1() {
	valve1.write(0);
}
void stop2() {
	valve2.write(0);
}
void stop3() {
	valve3.write(0);
}

void pomp_ctrl() {
	if (dualshock.rise(Start)) {
		pomp1.write(1);
		pompState1 = true;
		pomp2.write(1);
		pompState2 = true;
	} else {
		if (dualshock.rise(Triangle) && pompState1) {
			pomp1.write(0);
			valve1.write(1);
			valve_stop1.attach(&stop1, OPEN_TIME);
			pompState1 = false;
		}
		if (dualshock.rise(Square) && pompState2) {
			pomp2.write(0);
			valve2.write(1);
			valve_stop2.attach(&stop2, OPEN_TIME);
			pompState2 = false;
		}
	}
	if (dualshock.rise(Circle)) {
		if (pompState3) {
			pomp3.write(0);
			pomp4.write(0);
			valve3.write(1);
			valve_stop3.attach(&stop3, OPEN_TIME);
			pompState3 = false;
		} else {
			pomp3.write(1);
			pompState3 = true;
			pomp4.write(1);
			pompState4 = true;
		}
	}
}

bool uratRead() {
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
	joystick[Lx] = 2.0 * ((re_data[Lx] / 100.0) - 0.5);
	joystick[Rx] = 2.0 * ((re_data[Rx] / 100.0) - 0.5);
	dualshock.set_Data((re_data[sw2] << 8) + re_data[sw1]);
}

inline double myAbs(double a) {
	if (a > 0.0)
		return a;
	return (-1.0 * a);
}

inline int myAbs(int a) {
	if (a > 0)
		return a;
	return (-1 * a);
}

void normal_move() {

	double velocity = 0.75;
	if (dualshock.push(Cross))
		velocity = 0.4;

	motor.dir1 = motor.dir2 = (joystick[Ly] > 0.0) + 1;

	joystick[Ly] = myAbs(joystick[Ly]);
	motor.duty1 = motor.duty2 = (int) (joystick[Ly] * 999.9 * velocity);
	if (joystick[Ly] > 0.0) {
		if (dualshock.push(L1))
			motor.duty1 *= 0.75;
		if (dualshock.push(R1))
			motor.duty2 *= 0.75;
	} else {
		if (dualshock.push(L1))
			motor.duty2 *= 0.75;
		if (dualshock.push(R1))
			motor.duty1 *= 0.75;
	}
#ifdef print_debugger
#ifdef check_joystick
	pc.printf("L%4d R%4d dir %d , %d\n\r",motor.duty1,motor.duty2,motor.dir1,motor.dir2);
#endif
#endif
//足回り
	motor.drive_LP(1);
	motor.drive_LP(2);
}
void arm_ctrl() {
#ifdef double_push
	if ((dualshock.rise(Up)) && (arm_state_UP == false)) {
		arm_state_DOWN = false;
		arm_state_UP = true;
	} else if ((dualshock.rise(Down)) && (arm_state_DOWN == false)) {
		arm_state_UP = false;
		arm_state_DOWN = true;
	} else if ((arm_state_UP && dualshock.rise(Up))
			|| (arm_state_DOWN && dualshock.rise(Down))
			|| dualshock.push(Start))
//停止信号の確認
	arm_state_DOWN = arm_state_UP = false;
#else
	if (dualshock.push(Up)) {
		arm_state_DOWN = false;
		arm_state_UP = true;
	} else if (dualshock.push(Down | R3)) {
		arm_state_UP = false;
		arm_state_DOWN = true;
	} else
		arm_state_DOWN = arm_state_UP = false;
#endif

	if (arm_state_UP && (senser_up == 0)) {
		motor.dir3 = 1;
		led3.write(1);
	} else if (arm_state_DOWN && (senser_low == 0)) {
		motor.dir3 = 2;
		led3.write(1);
	} else {
		motor.dir3 = 0;
		led3.write(0);
	}

	double velocity = 1.0;
	if (dualshock.push(Cross))
		velocity = 0.4;

	motor.duty3 = (int) (999.9 * velocity);
	motor.drive(3);
}

#ifdef auto_ctrl
void automatic_move() {
//左がd1 右がd2
	unchar d1 = 0xFF, d2 = 0xFF;
	/*
	 * d1 = msg.data[distanceL];
	 * d2 = msg.data[distanceR];
	 * //戻り値を0 ~ 255に変換する
	 * d1 /= ~
	 */
//ただしKpは線形定数
//パラメータを指定
	const double Kp = 0.4;
	const unchar too_far = 200,too_dif = 50,too_near = 16;

	if (((d1 + d2)/2 > too_far)||(d1<too_near)||(d2<too_near)||((myAbs(d1-d2))>too_dif)) {
		motor.dir1 = motor.dir2 = 1;
		motor.duty1 = motor.duty2 = 300;
		motor.drive(1);
		motor.drive(2);
		return;
	} else if (d1 > d2) {
		//d1のほうが近い
		led1.write(1);
		motor.duty1 = 0;
		motor.duty2 = (d1 - d2) * Kp;
		motor.dir1 = motor.dir2 = 2;
	} else { //(d2>d1)
		led1.write(1);
		motor.duty1 = (d2 - d1) * Kp;
		motor.duty2 = 0;
		motor.dir1 = motor.dir2 = 2;
	}
}
void machine_move() {
	bool manual = true;
	if (dualshock.push(Select))
	manual = false;

	if (manual)
	normal_move();
	else
	automatic_move();
}
#else

void turn_L(int pwm) {
	if (dualshock.push(Cross)) {
		motor.duty1 = pwm / 2;
		motor.duty2 = pwm / 2;
	} else {
		motor.duty1 = pwm;
		motor.duty2 = pwm;
	}
	motor.dir1 = 0;
	motor.dir2 = 2;
	motor.allDrive_LP();
}
void turn_R(int pwm) {
	if (dualshock.push(Cross)) {
		motor.duty1 = pwm / 2;
		motor.duty2 = pwm / 2;
	} else {
		motor.duty1 = pwm;
		motor.duty2 = pwm;
	}
	motor.dir1 = 2;
	motor.dir2 = 0;
	motor.allDrive_LP();
}
void turn_L_S(int pwm) {
	if (dualshock.push(Cross)) {
		motor.duty1 = pwm / 2;
		motor.duty2 = pwm / 2;
	} else {
		motor.duty1 = pwm;
		motor.duty2 = pwm;
	}
	motor.dir1 = 1;
	motor.dir2 = 2;
	motor.allDrive_LP();
}
void turn_R_S(int pwm) {
	if (dualshock.push(Cross)) {
		motor.duty1 = pwm / 2;
		motor.duty2 = pwm / 2;
	} else {
		motor.duty1 = pwm;
		motor.duty2 = pwm;
	}
	motor.dir1 = 2;
	motor.dir2 = 1;
	motor.allDrive_LP();

}
void machine_move() {
	if (joystick[Ly] != 0.0) {
		normal_move();
	} else if (dualshock.push(L1 | R1 | L2 | R2 | Left | Right)) {
		if (dualshock.push(L2)) {
			turn_L_S(680);
		} else if (dualshock.push(R2)) {
			turn_R_S(680);
		} else if (dualshock.push(Left)) {
			turn_L_S(100);
		} else if (dualshock.push(Right)) {
			turn_R_S(100);
		} else if (dualshock.push(L1)) {
			turn_L(100);
		} else if (dualshock.push(R1)) {
			turn_R(100);
		}
	} else if (joystick[Rx] < 0.0) {
		turn_L((-1.0) * joystick[Rx] * 350.0);
	} else if (joystick[Rx] > 0.0) {
		turn_R(joystick[Rx] * 350.0);
	} else {
		normal_move();
	}
}
#endif

#ifdef CAN_BUS
void can_read()
{
	if(can.read(msg)) {

	}
}
#endif
void setup() {
#ifdef print_debug
	pc.baud(9600);
	pc.printf("program begin\n\r");
#endif

#ifdef CAN_BUS
	can.frequency(500 * 1000);
	msg.id = 17;
	can.attach(&can_read);
#endif
	slave.baud(38400);
	motor.init();
	led1.write(1);
//close valve
	valve1.write(0);
	valve2.write(0);
	valve3.write(0);
	pomp1.write(0);
	pomp2.write(0);
	pomp3.write(0);
	pomp4.write(0);
}
void loop() {
	int failed_counter = 0;
	if (uratRead()) {
		led2.write(1);
		input_val();
		machine_move();
		arm_ctrl();
		pomp_ctrl();
		wait_ms(15);
	} else {
		led2.write(0);
		failed_counter++;
		if (failed_counter > 2) {
			//通信が復帰することなく3回連続失敗
			motor.off();
		}
	}
}
int main() {
	setup();
	for (;;)
		loop();
}
