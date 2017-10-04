#ifndef MYSERIAL
#define MYSERIAL
#include "mbed.h"

typedef unsigned char unchar;

class MySerial
{
public:
	MySerial(Serial*hoge);
	unchar input();
private:
	Serial *srlio;
};

#endif // !MYSERIAL
