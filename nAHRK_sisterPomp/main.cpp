#include "mbed.h"
#include "myLIB/button.h"
#include "myLIB/mySerial.h"

int Ly = 0, Ry = 1, Lx = 2, Rx = 3, sw1 = 4, sw2 = 5, Sum = 6, end = 7;

//make instance
#ifdef print_debugger
Serial pc(p28, p27);
#endif
DigitalOut led1(p15), led2(p16), led3(p17);
Button dualshock;
Timeout valve_stop1, valve_stop2, valve_stop3, valve_stop4;

Serial slave(p9, p10);
MySerial FEP01TJ(&slave);

typedef unsigned char unchar;
unchar re_data[8] = { 0 };
const float OPEN_TIME = 0.3f;	//300ms

//port 4
DigitalOut air(P2_7);
//port 3+2
DigitalOut valve1(p24), valve3(P2_6), valve2(p23), valve4(P2_8);
//port SPI
DigitalOut pomp1(p5), pomp2(p6), pomp3(p7), pomp4(P0_22);

bool pompState1, pompState2, pompState3, pompState4;
bool arm_state_UP, arm_state_DOWN;
bool air_state;

//#define print_debugger

void stop1() {
	valve1.write(0);
}
void stop2() {
	valve2.write(0);
}
void stop3() {
	valve3.write(0);
}
void stop4() {
	valve4.write(0);
}

void pomp_ctrl() {
	if (dualshock.push(Circle)) {
		//close valve
#ifdef print_debugger
		pc.printf("open\n\r");
#endif
		valve1.write(0);
		valve2.write(0);
		valve3.write(0);
		valve4.write(0);
		//drive pomp
		pomp1.write(1);
		pomp2.write(1);
		pomp3.write(1);
		pomp4.write(1);
		pompState1 = pompState2 = pompState3 = pompState4 = true;
	} else {
		/*
		if (dualshock.push(L1)) {
			pomp1.write(0);
			if (pompState1) {
				valve1.write(1);
				valve_stop1.attach_us(stop1, OPEN_TIME);
				pompState1 = false;
			}
		}
		*/
		if (dualshock.push(R1)) {
			pomp2.write(0);
			if (pompState2) {
				valve2.write(1);
				valve_stop2.attach(&stop2, OPEN_TIME);
				pompState2 = false;
			}
		}
		/*
		if (dualshock.push(L2)) {
			pomp3.write(0);
			if (pompState3) {
				valve3.write(1);
				valve_stop3.attach_us(stop3, OPEN_TIME);
				pompState3 = false;
			}
		}*/
		if (dualshock.push(L1)) {//if (dualshock.push(R2)) {
			pomp4.write(0);
			if (pompState4) {
				valve4.write(1);
				valve_stop4.attach(stop4, OPEN_TIME);
				pompState4 = false;
			}
		}
	}
}

void air_ctrl() {
	if (dualshock.rise(Triangle)) {
		if (air_state == false) {
			led3.write(1);
			air.write(1);
			air_state = true;
		} else {
			led3.write(0);
			air.write(0);
			air_state = false;
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
			if (count > 17) {
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

void setUP() {
	led1.write(1);
#ifdef print_debugger
	pc.baud(9600);
	pc.printf("begin\n\r");
#endif
	slave.baud(38400);
	pompState1 = false;
	pompState2 = false;
	pompState3 = false;
	pompState4 = false;
	arm_state_DOWN = false;
	arm_state_UP = false;
	air_state = false;
	valve1.write(0);
	valve2.write(0);
	valve3.write(0);
	valve4.write(0);
	pomp1.write(0);
	pomp2.write(0);
	pomp3.write(0);
	pomp4.write(0);
	air.write(0);

}
void loop() {
	if (uratRead()) {
		led2.write(1);
		dualshock.set_data((re_data[sw2] << 8) + re_data[sw1]);
		air_ctrl();
		pomp_ctrl();
		wait_ms(10);
	} else {
		led2.write(0);
		//通信失敗してしまった
	}
}

int main() {
	setUP();
	for (;;)
		loop();
}
