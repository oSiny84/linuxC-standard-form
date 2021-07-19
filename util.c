#include "util.h"
#include <unistd.h>

void sleep_ms(unsigned long time)
{
	usleep(time*1000);
}

/*
Linux상의 Process 강제 종료(ctrl+c로 종료 되지 않을 경우.):
Ctrl + z 입력 후 kill -9 pid번호 로 종료.
*/