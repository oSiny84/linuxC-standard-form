#include "util.h"
#include <unistd.h>

void sleep_ms(unsigned long time)
{
	usleep(time*1000);
}