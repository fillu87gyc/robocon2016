#include "mbed.h"
#include "mySerial.h"
Timer stopper_;

MySerial::MySerial(Serial *hoge){
	srlio = hoge;
}
unchar MySerial::input()
{
	stopper_.start();
	stopper_.reset();
	while (stopper_.read_ms() < 50) {
		if (srlio->readable()) {
			return srlio->getc();
		}
	}
	//printf("time out\n\r");
	return 0;
}
