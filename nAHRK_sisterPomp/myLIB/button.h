#ifndef BUTTON_H_
#define BUTTON_H_

#include "mbed.h"
/*
dualshock 3		

button bit

-+------+-------+
 | bit0 | Select|
 | bit1 | L3	|
 | bit2 | R3	|
 | bit3 | Start	|
 | bit4 | Up	|
 | bit5 | Rigth	|
 | bit6 | Down	|
 | bit7 | Left	|
-+------+-------+--
 | bit8 | L2	|
 | bit9 | R2	|
 | bit10| L1	|
 | bit11| R1	+-----------+
 | bit12| ��	| Triangle	|
 | bit13| ��	| Circle	|
 | bit14| �~	| Cross		|
 | bit15| ��	| Square	|
-+------+-------+-----------+


*/

#define Select	 (0x01 << 0)
#define L3		 (0x01 << 1)
#define R3		 (0x01 << 2)
#define Start	 (0x01 << 3)
#define Up		 (0x01 << 4)
#define Right	 (0x01 << 5)
#define Down	 (0x01 << 6)
#define Left	 (0x01 << 7)
				  	   	  
#define L2		 (0x01 << 8)
#define R2		 (0x01 << 9)
#define L1		 (0x01 << 10)
#define R1		 (0x01 << 11)
#define Triangle (0x01 << 12)
#define Circle	 (0x01 << 13)
#define Cross	 (0x01 << 14)
#define Square	 (0x01 << 15)

class Button
{
public:
	Button();
	void set_data(int);
	bool rise(int);
	bool push(int);
private:
	int button;
	int button_old;
};


#endif // !BUTTON_H_
