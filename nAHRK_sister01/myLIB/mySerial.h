#ifndef MYSERIAL
#define MYSERIAL
#include "mbed.h"

typedef unsigned char unchar;
#define packet_size (9 - 1) //It is better one less because packet-header is processed by "while".

//class Serial { public: void putc() {}unchar getc() {}void baud(int) {} int readable() {} Serial(int a, int b) {} };

class MySerial
{
public:
	MySerial(Serial*hoge);
	unchar input();
private:
	Serial *srlio;
};

#endif // !MYSERIAL
