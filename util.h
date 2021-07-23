#ifndef UTIL_H
#define UTIL_H

void sleep_ms(unsigned long time);

#define printfIt() printf("Call %s at %s::%05d \r\n", __FUNCTION__, __FILE__, __LINE__);

#endif