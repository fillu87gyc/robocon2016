#include "mbed.h"
#include "mySerial.h"
Timer stopper_;

MySerial::MySerial(Serial *hoge) {
	stopper_.start();
	srlio = hoge;
}
unchar MySerial::input() {
	stopper_.reset();
	while (stopper_.read_ms() < 5) {
		if (srlio->readable()) {
			return srlio->getc();
		}
	}
	//printf("time out\n\r");
	return 0;
}
