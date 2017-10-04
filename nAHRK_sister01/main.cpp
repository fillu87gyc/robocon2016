#include "mbed.h"
#include "myLIB/button.h"
#include "Motor/Motor.h"

//#define mini_mode

int Ly = 0, Ry = 1, Lx = 2, Rx = 3, sw1 = 4, sw2 = 5, Sum = 6, end = 7;

//make instance
Serial pc(p28, p27);
CAN can(p30, p29);
CANMessage msg;
DigitalOut led1(p15), led2(p16), led3(p17);
Button dualshock;

DigitalIn senser_up(p19);
DigitalIn senser_low(p20);

//#define print_debugger

#ifdef mini_mode
//port 4
DigitalOut air(P2_6);
//port 3+2
DigitalOut valve1(p24), valve2(P2_7), valve3(p23), valve4(P2_8);
//port SPI
DigitalOut pomp1(p5), pomp2(p6), pomp3(p7), pomp4(P0_22);

Timeout valve_stop1, valve_stop2, valve_stop3, valve_stop4;
#endif
//port 1
Motor motor;	//(p22 (0x01<<11) )

#ifdef mini_mode

#define OPEN_TIME 0.2f

bool pompState1,pompState2,pompState3, pompState4;
bool arm_state_UP ,arm_state_DOWN;
bool air_state;

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
#ifdef print_debugger
#ifdef pomp_check
		pc.printf("open\n\r");
#endif
#endif
		//close valve
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
		if (dualshock.push(L1)) {
			pomp1.write(0);
			if (pompState1) {
				valve1.write(1);
				valve_stop1.attach(stop1, OPEN_TIME);
				pompState1 = false;
			}
		}
		if (dualshock.push(R1)) {
			pomp2.write(0);
			if (pompState2) {
				valve2.write(1);
				valve_stop2.attach(stop2, OPEN_TIME);
				pompState2 = false;
			}
		}
		if (dualshock.push(L2)) {
			pomp3.write(0);
			if (pompState3) {
				valve3.write(1);
				valve_stop3.attach(stop3, OPEN_TIME);
				pompState3 = false;
			}
		}
		if (dualshock.push(R2)) {
			pomp4.write(0);
			if (pompState4) {
				valve4.write(1);
				valve_stop4.attach(stop4, OPEN_TIME);
				pompState4 = false;
			}
		}
	}
}
#endif
void arm_ctrl()
{

	if (dualshock.push(Up) && (senser_up == 0)) {
		motor.dir1 = 2;
		led3.write(1);
	} else if (dualshock.push(Down) && (senser_low == 0)) {
		motor.dir1 = 1;
		led3.write(1);
	} else {
		motor.dir1 = 0;
		led3.write(0);
	}

	double velocity = 1.0;
	if (dualshock.push(Cross))
		velocity = 0.4;

	motor.duty1 = (int)(999.9 * velocity);
	motor.drive(1);
}
#ifdef mini_mode
void air_ctrl()
{
	if(dualshock.rise(Circle))
	{
		if(air_state == false){
			led2.write(1);
			air.write(1);
			air_state = true;
		}
		else {
			led2.write(0);
			air.write(0);
			air_state = false;
		}
	}
}
#endif
void can_read() {
	if (can.read(msg)) {
		led1.write(1);
		led2.write(msg.data[7]);
#ifdef print_debugger
#ifdef check_CAN
		pc.printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n\r",
				msg.data[0],msg.data[1],msg.data[2],msg.data[3],
				msg.data[4],msg.data[5],msg.data[6],msg.data[7]);
#endif
#endif
		wait_ms(15);
		led1.write(0);
		led2.write(0);
		dualshock.set_data((msg.data[sw2] << 8) + msg.data[sw1]);
#ifdef mini_mode
		air_ctrl();
		pomp_ctrl();
#endif
		arm_ctrl();

	}
}

void setUP()
{
	can.frequency(500 * 1000);     //can通信のクロックレート
	msg.id = 17;
	can.attach(&can_read);
	motor.init();
#ifdef mini_mode
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
#endif
#ifdef print_debugger
	pc.printf("begin\n\r");
#endif
}


int main()
{
	setUP();
	for (;;);
}
